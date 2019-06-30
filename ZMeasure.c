#include <utility.h>
#include <ansi_c.h>
#include <userint.h>
#include <asynctmr.h>

#include "fixedziAPI.h"
#include "meastypes.h"

#include "ZMeasure.h"
#include "measurementSetup.h"



// Function prototypes
void deleteMeasurementLegacy(struct MeasurementLegacy* MeasurementLegacy);
void initializePanels(ZMeasure* zmeasure);
void destroyAllPanels(PrimaryPanels* panels);

void uiConnectToZurich();



int main (int argc, char *argv[])
{
	ZMeasure* zmeasure = allocateSystemVars();
	initializePanels(zmeasure);
	
	RunUserInterface ();
	
	// Close UI async timer
	deleteUITimerThread(zmeasure);

	destroyAllPanels(zmeasure->panels);
	deleteSystemVars(zmeasure);
	
	return 0;
}


// Initialization and destruction functions

void initializePanels(ZMeasure* zmeasure)
{
	PrimaryPanels* panels = zmeasure->panels;
	panels->main = LoadPanel (0, "ZMeasure.uir", MAINP);
	SetPanelAttribute(panels->main, ATTR_CALLBACK_DATA, zmeasure);
	
	panels->about = LoadPanel(panels->main, "ZMeasure.uir", ABOUTP);
	
	panels->newZConn = LoadPanel(panels->main, "ZMeasure.uir", NEWZCONNP);
	SetPanelAttribute(panels->newZConn, ATTR_CALLBACK_DATA, zmeasure);
	
	// Display some starting panels
	DisplayPanel(panels->main);
}

void destroyAllPanels(PrimaryPanels* panels)
{
	DiscardPanel(panels->newZConn);
	DiscardPanel(panels->about); 
	DiscardPanel(panels->main);
}



void userRequestedExit(ZMeasure* zmeasure)
{
	// Hide the main panel so it looks like we quit faster than we actually do
	HidePanel(zmeasure->panels->main);
	QuitUserInterface(0);
}

void enableAllZurichUIControls(int panel)
{
	int arrayHandle;
	
	// Osc Freqs
	arrayHandle = GetCtrlArrayFromResourceID(panel, FREQ);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, 0);
	
	// Frequency locked indicator
	arrayHandle = GetCtrlArrayFromResourceID(panel, FREQLOCK);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, 0);
	
	// Demod Oscs
	arrayHandle = GetCtrlArrayFromResourceID(panel, OSCS);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, 0);
	
	// Harmonic
	arrayHandle = GetCtrlArrayFromResourceID(panel, HARM);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, 0);
	
	// Demod phase offset
	arrayHandle = GetCtrlArrayFromResourceID(panel, PHASE);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, 0);
	
	// Auto phase button
	arrayHandle = GetCtrlArrayFromResourceID(panel, AUTOPHASE);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, 0);
	
	// Input signal
	arrayHandle = GetCtrlArrayFromResourceID(panel, SIG);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, 0);
	
	// Filter order
	arrayHandle = GetCtrlArrayFromResourceID(panel, ORDER);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, 0);
	
	
}


/***** UI management functions *****/

// Called to change which zurich device is displayed in the main UI
void uiSelectActiveZurich(ZMeasure* zmeasure, ZurichConn* zurich)
{
	// TODO: Implement this function
}

// Establish a connection to the zurich for use in the main UI window
void uiConnectToZurich(ZMeasure* zmeasure)
{
	int panel = zmeasure->panels->newZConn;
	
	// Read connection information from the UI
	char address[64];
	uint16_t port;
	char device[8];
	
	GetCtrlVal(panel, NEWZCONNP_SERVER, address);
	GetCtrlVal(panel, NEWZCONNP_PORT, &port);
	GetCtrlVal(panel, NEWZCONNP_DEVICE, device);
	
	ZurichConnDef* connDef = newZurichConnDef(address, port, device);
	
	// TODO: Handle unsuccessful connections better
	struct ZurichConn* zurich = newZurichConn(connDef);
	addZurichConnToZMeasure(zmeasure, zurich);
	
	// Update UI
	char connName[128];
	getConnName(connDef, connName);
	// Cast ZurichConn* to char* because the list control only supports char* as a type
	InsertListItem(zmeasure->panels->main, MAINP_CONNECTIONS, -1, connName, (int)zurich);
	
	SetCtrlAttribute(zmeasure->panels->main, MAINP_DISCONNECT, ATTR_DIMMED, 0);
	
	// Set up active connection
	deleteUITimerThread(zmeasure);
	newUITimerThread(zmeasure, zurich->connDef);
	
	enableAllZurichUIControls(zmeasure->panels->main);
}


int setZIValueAdvD(ZurichConn* zurich, int panel, int control, int handleArray, char* fmtpath)
{
	int index;
	int arrayHandle = GetCtrlArrayFromResourceID(panel, FREQ);
	GetCtrlArrayIndex(arrayHandle, panel, control, &index);
	if (index != -1) {
		char strvalue[32];
		double value;
		char path[MAX_PATH_LEN];
		sprintf(path, "/%s/oscs/%d/freq", zurich->connDef->device, index);
		GetCtrlVal(panel, control, strvalue);
		int retval = readNumberScientific(strvalue, &value);
		if (retval == 0) {
			ziAPISetValueD(zurich->conn, path, value);
			// Change active item
			int nextArrayHandle = GetCtrlArrayFromResourceID(panel, OSCS);
			int nextControl = GetCtrlArrayItem(nextArrayHandle, index);
			SetActiveCtrl(panel, nextControl);
			return 1;
		}
	}
	
	return 0;
}

int setZIValueD(ZurichConn* zurich, int panel, int control, int handleArray, char* fmtpath)
{
	int index;
	int arrayHandle = GetCtrlArrayFromResourceID(panel, handleArray);
	GetCtrlArrayIndex(arrayHandle, panel, control, &index);
	if (index != -1) {
		double value;
		char path[MAX_PATH_LEN];
		sprintf(path, fmtpath, zurich->connDef->device, index);
		GetCtrlVal(panel, control, &value);
		ziAPISetValueD(zurich->conn, path, value);
		return 1;
	}
	
	return 0;
}

int setZIValueI(ZurichConn* zurich, int panel, int control, int handleArray, char* fmtpath)
{
	int index;
	int arrayHandle = GetCtrlArrayFromResourceID(panel, handleArray);
	GetCtrlArrayIndex(arrayHandle, panel, control, &index);
	if (index != -1) {
		int value;
		char path[MAX_PATH_LEN];
		sprintf(path, fmtpath, zurich->connDef->device, index);
		GetCtrlVal(panel, control, &value);
		ziAPISetValueI(zurich->conn, path, value);
		return 1;
	}
	
	return 0;
}


// Called whenever the user changes one of the lock-in parameters to update that
// parameter on the actual tool
void setZIValue(ZurichConn* zurich, int panel, int control)
{
	// Check each control array to figure out which element/index was modified
	// Ordered in approximately decreasing order of use to improve performance
	// by the smallest of margins
	
	// Oscillator frequency
	if (setZIValueAdvD(zurich, panel, control, FREQ, "/%s/oscs/%d/freq")) {
		return;
	}
	
	// Demod oscillator
	if (setZIValueI(zurich, panel, control, OSCS, "/%s/demods/%d/oscselect")) {
		return;
	}
	
	// Demod phase shift
	if (setZIValueD(zurich, panel, control, PHASE, "/%s/demods/%d/phaseshift")) {
		return;
	}
	
	// Demod input
	if (setZIValueI(zurich, panel, control, SIG, "/%s/demods/%d/adcselect")) {
		return;
	}
	
	// Demod filter order
	if (setZIValueI(zurich, panel, control, ORDER, "/%s/demods/%d/order")) {
		return;
	}
	
	// Demod harmonic
	if (setZIValueI(zurich, panel, control, HARM, "/%s/demods/%d/harmonic")) {
		return;
	}
}

/***** UI Callbacks *****/

int CVICALLBACK mainPanel_CB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
		userRequestedExit((ZMeasure*) callbackData);
	if (event == EVENT_PANEL_SIZING) {
		
		// Get the current panel size so we can decide if we want to restrict it
		Rect panelRect;
		GetPanelEventRect(eventData2, &panelRect);
		
		// Enforce a minimum width
		if (panelRect.width < 500) {
			panelRect.width = 500;
		}
		
		// Enforce a minimum height
		if (panelRect.height < 400) {
			panelRect.height = 400;
		}
		
		// Set control positions
		SetCtrlAttribute(panel, MAINP_SPLITTER, ATTR_HEIGHT, panelRect.height);
		
		SetPanelEventRect(eventData2, panelRect);   
	}
		
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
	ZMeasure* zmeasure;
	GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &zmeasure);
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control) {
				case NEWZCONNP_CONNECT:
					uiConnectToZurich(zmeasure);
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
	// Load the ZMeasure struct
	struct ZMeasure* zmeasure;
	GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &zmeasure);
	
	// Determine which menu item was selected
	switch(menuItem) {
		case MENUBAR_HELP_ABOUT:
			DisplayPanel(zmeasure->panels->about);
			break;
		case MENUBAR_SETUP_CONNECTION_NEW:
			DisplayPanel(zmeasure->panels->newZConn);
			break;
	}
}


// Called from the "Exit" menu item to quit the program
void CVICALLBACK exit_CB (int menuBar, int menuItem, void *callbackData,int panel)
{
	ZMeasure* zmeasure;
	GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &zmeasure);
	userRequestedExit(zmeasure);
}

int CVICALLBACK setZIValue_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	ZMeasure* zmeasure;
	GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &zmeasure);
	int fakeptr;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, MAINP_CONNECTIONS, &fakeptr);
			setZIValue((ZurichConn*)fakeptr, panel, control);
			break;
	}
	return 0;
}

int CVICALLBACK autophase_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			autophase(panel, control);
			break;
	}
	return 0;
}

int CVICALLBACK enableDemod_CB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK enableOutput_CB (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

