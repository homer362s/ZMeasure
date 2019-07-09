#include "measurement_u.h"
#include <utility.h>
#include <ansi_c.h>
#include <userint.h>
#include <asynctmr.h>

#include "fixedziAPI.h"
#include "meastypes.h"

#include "ZMeasure.h"
#include "measurementSetup.h"

#include "measurement.h"


// Function prototypes
void deleteMeasurementLegacy(struct MeasurementLegacy* MeasurementLegacy);
void initializePanels(ZMeasure* zmeasure);
void destroyAllPanels(PrimaryPanels* panels);
static ZurichConn* uiGetActiveZurichConn(ZMeasure* zmeasure);

static void enableAllZurichUIControls(int panel);
static void disableAllZurichUIControls(int panel);
static void setStateAllZurichUIControls(int panel, int state);

void uiConnectToZurich(ZMeasure* zmeasure);



int main(void)
{
	initMeasNodes();
	ZMeasure* zmeasure = allocateSystemVars();
	initializePanels(zmeasure);
	
	RunUserInterface ();
	
	// Close UI async timer
	deleteUITimerThread(zmeasure);

	destroyAllPanels(zmeasure->panels);
	deleteSystemVars(zmeasure);
	deleteMeasNodes();
	Delay(5);
	
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
	setStateAllZurichUIControls(panel, 0);
}

void disableAllZurichUIControls(int panel)
{
	setStateAllZurichUIControls(panel, 1);
}

void setStateAllZurichUIControls(int panel, int state)
{
	int arrayHandle;
	
	// Osc Freqs
	arrayHandle = GetCtrlArrayFromResourceID(panel, FREQ);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, state);
	
	// Frequency locked indicator
	arrayHandle = GetCtrlArrayFromResourceID(panel, FREQLOCK);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, state);
	
	// Demod Oscs
	arrayHandle = GetCtrlArrayFromResourceID(panel, OSCS);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, state);
	
	// Harmonic
	arrayHandle = GetCtrlArrayFromResourceID(panel, HARM);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, state);
	
	// Demod phase offset
	arrayHandle = GetCtrlArrayFromResourceID(panel, PHASE);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, state);
	
	// Auto phase button
	arrayHandle = GetCtrlArrayFromResourceID(panel, AUTOPHASE);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, state);
	
	// Input signal
	arrayHandle = GetCtrlArrayFromResourceID(panel, SIG);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, state);
	
	// Filter order
	arrayHandle = GetCtrlArrayFromResourceID(panel, ORDER);
	SetCtrlArrayAttribute(arrayHandle, ATTR_DIMMED, state);
}


/***** UI management functions *****/

// Called to change which zurich device is displayed in the main UI
void uiSelectActiveZurich(ZMeasure* zmeasure, ZurichConn* zurich)
{
	deleteUITimerThread(zmeasure);
	newUITimerThread(zmeasure, zurich->connDef);
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
	
	struct ZurichConn* zurich = newZurichConn(connDef);
	if (zurich == 0) {
		printf("Connection failed.\nSomething needs to be fixed.\nIt's probably your fault.\n");
		deleteZurichConnDef(connDef);
		return;
	}
	addZurichConnToZMeasure(zmeasure, zurich);
	
	// Update UI
	char connName[128];
	getConnName(connDef, connName);
	// Cast ZurichConn* to int because the list control doesn't support void* as a type
	InsertListItem(zmeasure->panels->main, MAINP_CONNECTIONS, -1, connName, (int)zurich);
	
	SetCtrlAttribute(zmeasure->panels->main, MAINP_DISCONNECT, ATTR_DIMMED, 0);
	
	// Set up active connection
	uiSelectActiveZurich(zmeasure, zurich);
	
	enableAllZurichUIControls(zmeasure->panels->main);
}

// Terminate an active zurich connection in the main UI window
void uiDisconnectFromZurich(ZMeasure* zmeasure)
{
	ZurichConn* zurich = uiGetActiveZurichConn(zmeasure);
	if (zurich == 0) {
		return;
	}
	
	int index;
	GetCtrlIndex(zmeasure->panels->main, MAINP_CONNECTIONS, &index);
	if (index < 0) {
		return;
	}
	
	removeZurichConnFromZMeasure(zmeasure, zurich);
	
	DeleteListItem(zmeasure->panels->main, MAINP_CONNECTIONS, index, 1);
	deleteZurichConn(zurich);
	
	// Establish UI for newly selected zurich device
	zurich = uiGetActiveZurichConn(zmeasure);
	if (zurich == 0) {
		disableAllZurichUIControls(zmeasure->panels->main);
		return;
	}
	
	uiSelectActiveZurich(zmeasure, zurich);
}


// Returns a ZurichConn for the currently selected zurich device
static ZurichConn* uiGetActiveZurichConn(ZMeasure* zmeasure)
{
	int index;
	GetCtrlIndex(zmeasure->panels->main, MAINP_CONNECTIONS, &index);
	if (index < 0) {
		return 0;
	}
	
	ZurichConn* zurich;
	GetCtrlVal(zmeasure->panels->main, MAINP_CONNECTIONS, (int*)(&zurich));
	
	return zurich;
}

int setZIValueAdvD(ZurichConn* zurich, int panel, int control, int handleArray, char* fmtpath)
{
	int index;
	int arrayHandle = GetCtrlArrayFromResourceID(panel, handleArray);
	GetCtrlArrayIndex(arrayHandle, panel, control, &index);
	if (index != -1) {
		char strvalue[32];
		double value;
		char path[MAX_PATH_LEN];
		sprintf(path, fmtpath, zurich->connDef->device, index);
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

// Called to rearrange panel controls to properly fit a given panel size
void fixControlPositions(int panel)
{
	int pwidth, pheight, height, top;
	GetPanelAttribute(panel, ATTR_WIDTH, &pwidth);
	GetPanelAttribute(panel, ATTR_HEIGHT, &pheight);
	
	SetCtrlAttribute(panel, MAINP_VERTSPLIT, ATTR_HEIGHT, pheight);
	
	GetCtrlAttribute(panel, MAINP_MEASUREMENTS, ATTR_TOP, &top);
	height = pheight - top - 35;
	SetCtrlAttribute(panel, MAINP_MEASUREMENTS, ATTR_HEIGHT, height);
	SetCtrlAttribute(panel, MAINP_NEWMEAS, ATTR_TOP, pheight-30);
	SetCtrlAttribute(panel, MAINP_DELETEMEAS, ATTR_TOP, pheight-30);
}

/***** UI Callbacks *****/
int CVICALLBACK mainPanel_CB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	// When the user clicks the 'X' in the top right corner to close the window
	if (event == EVENT_CLOSE) {
		userRequestedExit((ZMeasure*) callbackData);
		return 0;
	}
	
	// Whever the panel is resized by dragging a corner or side
	if (event == EVENT_PANEL_SIZING) {
		
		// Get the current panel size so we can decide if we want to restrict it
		Rect panelRect;
		GetPanelEventRect(eventData2, &panelRect);
		
		// Enforce a minimum width
		if (panelRect.width < 500) {
			panelRect.width = 500;
		}
		
		// Enforce a minimum height
		if (panelRect.height < 525) {
			panelRect.height = 525;
		}
		
		SetPanelEventRect(eventData2, panelRect); 
		
		fixControlPositions(panel);
		return 0;
	}
	
	// When the user un-maximizes the window
	if (event == EVENT_PANEL_RESTORE) {
		fixControlPositions(panel);
		return 0;
	}
	
	// When the user maximizes the window
	if (event == EVENT_PANEL_MAXIMIZE) {
		fixControlPositions(panel);
		return 0;
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
	if (event == EVENT_COMMIT)
	{
		ZMeasure* zmeasure;
		GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &zmeasure);
		ZurichConn* zurich;
		GetCtrlVal(panel, MAINP_CONNECTIONS, (int*)(&zurich));
		setZIValue(zurich, panel, control);
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

int CVICALLBACK enableDemod_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK enableOutput_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK editMeasurement_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			switch (eventData1) {
				case LEFT_DOUBLE_CLICK:
				case ENTER_KEY:
					int index = eventData2;
					Measurement* measurement;
					GetTreeItemAttribute(panel, MAINP_MEASUREMENTS, index, ATTR_CTRL_VAL, (int*)&measurement);
					raiseMeasurementPanel(measurement);
			}
			// Figure out which measurement this is
			break;
	}
	return 0;
}


int CVICALLBACK manageConnections_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	ZMeasure* zmeasure;
	GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &zmeasure);
	
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control) {
				case MAINP_CONNECT:
					DisplayPanel(zmeasure->panels->newZConn);
					break;
				case MAINP_DISCONNECT:
					uiDisconnectFromZurich(zmeasure);
					//int treeSelectedIndex, treeSelectedRoot;
					//int fakenode;
					//struct ZurichNode* node;
					//struct ZurichData* zurich;

					// Get the selected item index
					//GetActiveTreeItem(panel, ZNODESP_SETTINGTREE, &treeSelectedIndex);
					
					// Get the root node for the selected item
					//GetTreeItem (panel, ZNODESP_SETTINGTREE, VAL_ANCESTOR, treeSelectedIndex, VAL_LAST, VAL_NEXT_PLUS_SELF, 0, &treeSelectedRoot);
					
					// Value will be -1 if the node is the root
					//if(treeSelectedRoot == -1) {
					//	treeSelectedRoot = treeSelectedIndex;
					//}
					
					// Get the ZurichNode for the root
					//GetTreeItemAttribute(panel, ZNODESP_SETTINGTREE, treeSelectedRoot, ATTR_CTRL_VAL, &fakenode);
					//node = (struct ZurichNode*) fakenode;
					//zurich = getZurichDataFromNode(MeasurementLegacy, node);
					
					// Disconnect from the selected zurich
					//disconnectFromZurich(MeasurementLegacy, zurich);
					
					// Remove the root item from the tree. 
					//DeleteListItem(panel, MAINP_SETTINGTREE, treeSelectedRoot, 1);
					
					// If there are no more nodes then disable the disconnect button
					//if(main->connCount <= 0) {
					//	SetCtrlAttribute(panel, MAINP_DISCONNECT, ATTR_DIMMED, 1);
					//}
					break;
			}
			break;
	}
	return 0;
}


int CVICALLBACK activeZurichChange_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_VAL_CHANGED) {
		ZMeasure* zmeasure;
		GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &zmeasure);
		
		ZurichConn* zurich;
		GetCtrlVal(panel, MAINP_CONNECTIONS, (int*)(&zurich));
		
		uiSelectActiveZurich(zmeasure, zurich);
	}

	return 0;
}
