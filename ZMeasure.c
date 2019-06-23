
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "ZMeasure.h"

#include "toolbox.h"

#include "fixedziAPI.h"
#include "measurementSetup.h"


// Function prototypes
struct Measurement* allocateMeasurement();
void deleteMeasurement(struct Measurement* measurement);
void initializePanels(struct Measurement* measurement);
void destroyAllPanels(struct PanelHandles* panels);



int main (int argc, char *argv[])
{
	struct Measurement* measurement = allocateMeasurement();
	initializePanels(measurement);
	
	RunUserInterface ();

	destroyAllPanels(measurement->panels);
	deleteMeasurement(measurement);
	
	return 0;
}


// Initialization and destruction functions


void initializePanels(struct Measurement* measurement)
{
	measurement->panels->main = LoadPanel (0, "ZMeasure.uir", MAINP);
	SetPanelAttribute(measurement->panels->main, ATTR_CALLBACK_DATA, measurement);
	
	measurement->panels->about = LoadPanel(measurement->panels->main, "ZMeasure.uir", ABOUTP);
	
	measurement->panels->zconn = LoadPanel(measurement->panels->main, "ZMeasure.uir", ZCONNP);
	SetPanelAttribute(measurement->panels->zconn, ATTR_CALLBACK_DATA, measurement);
	
	measurement->panels->znodes = LoadPanel(measurement->panels->main, "ZMeasure.uir", ZNODESP);
	SetPanelAttribute(measurement->panels->znodes, ATTR_CALLBACK_DATA, measurement);
	
	
	DisplayPanel(measurement->panels->main);
}

void destroyAllPanels(struct PanelHandles* panels)
{
	DiscardPanel(panels->znodes);
	DiscardPanel(panels->zconn);
	DiscardPanel(panels->about); 
	DiscardPanel(panels->main);
}



void userRequestedExit()
{
	//disconnectFromZurich();
	QuitUserInterface(0);
	
}

/***** UI Callbacks *****/

int CVICALLBACK mainPanel_CB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
		userRequestedExit();
	return 0;
}

int CVICALLBACK subpanel_CB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_KEYPRESS)
		switch(eventData1) {
			case VAL_ESC_VKEY:
				HidePanel(panel);
		}
	return 0;
}

int CVICALLBACK connect_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	struct Measurement* measurement;
	GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measurement);
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control) {
				case ZCONNP_CONNECT:
					connectToZurich(measurement);
					HidePanel(panel);
					break;
			}
			break;
	}
	return 0;
}


int CVICALLBACK closePanel_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel(panel);
			break;
	}
	return 0;
}


void CVICALLBACK openPanel_CB (int menuBar, int menuItem, void *callbackData, int panel)
{
	struct Measurement* measurement;
	GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measurement);
	switch(menuItem) {
		case MENUBAR_HELP_ABOUT:
			DisplayPanel(measurement->panels->about);
			break;
		case MENUBAR_SETUP_ZURICH:
			DisplayPanel(measurement->panels->znodes);
			break;
			
	}
}

void CVICALLBACK exit_CB (int menuBar, int menuItem, void *callbackData,int panel)
{
	userRequestedExit();
}
