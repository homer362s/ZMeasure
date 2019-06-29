#include <utility.h>
#include <ansi_c.h>
#include <asynctmr.h>

#include "fixedziAPI.h"
#include "meastypes.h"

#include "ZMeasure.h"
#include "zurichNodeTree.h"
#include "measurementSetup.h"


int CVICALLBACK updateZurichUIControls(int reserved, int timerId, int event, void *callbackData, int eventData1, int eventData2);
void formatNumberScientific(char* str, double value);

// Create main system struct
ZMeasure* allocateSystemVars()
{
	ZMeasure* zmeasure = malloc(sizeof(ZMeasure));
	
	zmeasure->activeConn = malloc(sizeof(ZMeasure));
	zmeasure->activeConn->timer = 0;
	zmeasure->activeConn->conn = 0;
	
	zmeasure->panels = malloc(sizeof(PrimaryPanels));
	for(int i = 0;i < MAX_DEVICE_CONNECTIONS;i++) {
		zmeasure->connections[i] = 0;
	}
	zmeasure->connCount = 0;

	return zmeasure;
}

// Free the main system struct
void deleteSystemVars(ZMeasure* zmeasure)
{
	free(zmeasure->activeConn);
	
	// Delete each connection
	for(int i = 0;i < zmeasure->connCount;i++) {
		deleteZurichConn(zmeasure->connections[i]);
	}
	
	// Delete remaining variables
	free(zmeasure->panels);
	free(zmeasure);
}

// This function is more complicated than I think it should need to be.
// When calling DiscardAsyncTimer() we seem to trigger another call to the async timer callback
// which then causes a "general protection fault" on a call to a ziAPI function. 
// By disabling the timer and then waiting a period of time we can be reasonably sure all the
// callbacks have completed and we can disable it without this happening. 
// It seems like we should just be able to call DiscardAsyncTimer() and be done with it though.
void deleteUITimerThread(ZMeasure* zmeasure)
{
	if (zmeasure->activeConn->conn) {
		// Disable the timer in question
		SetAsyncTimerAttribute(zmeasure->activeConn->timer, ASYNC_ATTR_ENABLED, 0);
		
		// Get time to wait to be reasonably sure the async callback is finished
		double deltaTime;
		GetAsyncTimerAttribute(zmeasure->activeConn->timer, ASYNC_ATTR_DELTA_TIME, &deltaTime);
		
		// Delay 3 times this time, as a safety margin.
		Delay(deltaTime*3);
		
		// Discard the async timer
		DiscardAsyncTimer(zmeasure->activeConn->timer);
		
		// Clean up the memory
		deleteZurichConn(zmeasure->activeConn->conn);
		zmeasure->activeConn->conn = 0;
	}
}

void newUITimerThread(ZMeasure* zmeasure, ZurichConnDef* oldConnDef)
{
	ZurichConnDef* connDef = copyZurichConnDef(oldConnDef);
	ZurichConn* conn = newZurichConn(connDef);
	zmeasure->activeConn->conn = conn;
	zmeasure->activeConn->timer = NewAsyncTimer(1, -1, 1, updateZurichUIControls, zmeasure);
}

ZurichConnDef* newZurichConnDef(char* address, uint16_t port, char* device)
{
	size_t len;
	ZurichConnDef* connDef = malloc(sizeof(ZurichConnDef));
	
	// Allocate space for the address and copy the address
	len = strlen(address);
	connDef->address = malloc((len+1) * sizeof(char));
	strcpy(connDef->address, address);
	
	// Copy the port number
	connDef->port = port;
	
	// Allocate space for the name and copy the name
	len = strlen(device);
	connDef->device = malloc((len+1) * sizeof(char));
	strcpy(connDef->device, device);
	
	return connDef;
}


// Returns (in char* name) a formatted string representing the connection name
// char* name must have enough space to hold this string
void getConnName(ZurichConnDef* connDef, char* name)
{
	sprintf(name, "%s  (%s:%d)", connDef->device, connDef->address, connDef->port);
}

void deleteZurichConnDef(ZurichConnDef* connDef)
{
	free(connDef->address);
	free(connDef->device);
	free(connDef);
}

ZurichConnDef* copyZurichConnDef(ZurichConnDef* connDefOri)
{
	size_t len;
	ZurichConnDef* connDefCpy = malloc(sizeof(ZurichConnDef));
	
	// Allocate space for the address and copy the address
	len = strlen(connDefOri->address);
	connDefCpy->address = malloc((len+1) * sizeof(char));
	strcpy(connDefCpy->address, connDefOri->address);
	
	// Copy the port number
	connDefCpy->port = connDefOri->port;
	
	// Allocate space for the name and copy the name
	len = strlen(connDefOri->device);
	connDefCpy->device = malloc((len+1) * sizeof(char));
	strcpy(connDefCpy->device, connDefOri->device);
	
	return connDefCpy;
}

// Create a new Zurich Conn struct and establish a new zurich connection
ZurichConn* newZurichConn(ZurichConnDef* connDef)
{
	ZurichConn* zurich = malloc(sizeof(ZurichConn));
	
	// Initialize ZIConnection.
	zurich->retVal = ziAPIInit(&zurich->conn);
	if (zurich->retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(zurich->retVal, &zurich->errBuffer, NULL);
		fprintf(stderr, "Can't init Connection: %s\n", zurich->errBuffer);
		return 0;
	}
	
	
	// Connect to data server
	zurich->retVal = ziAPIConnectEx(zurich->conn, connDef->address, connDef->port, ZI_API_VERSION_6, NULL);
	if (zurich->retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(zurich->retVal, &zurich->errBuffer, NULL);
		fprintf(stderr, "Error, can't connect to the Data Server: `%s`.\n", zurich->errBuffer);
		return 0;
	} 
	
	
	// Allocate an event struct
	zurich->event = ziAPIAllocateEventEx();
	
	
	// Save the connection definition 
	zurich->connDef = connDef;
	
	// Return the ZurichConn struct
	return zurich;
}

void deleteZurichConn(ZurichConn* zurich)
{
	if (zurich) {
		deleteZurichConnDef(zurich->connDef);
		ziAPIUnSubscribe(zurich->conn, "*"); 
		ziAPIDeallocateEventEx(zurich->event);
		ziAPIDisconnect(zurich->conn);
		ziAPIDestroy(zurich->conn);
		free(zurich);
	}
}

// Returns the zurich data assocated with a given node tree
struct ZurichData* getZurichDataFromNode(struct MeasurementLegacy* MeasurementLegacy, struct ZurichNode* node)
{
	for(int i = 0;i < MeasurementLegacy->connectionCount;i++) {
		if (MeasurementLegacy->zurich[i]->tree == node)
			return MeasurementLegacy->zurich[i];
	}
	return 0;
}


void updateControlStrD(ZurichConn* zurich, int panel, int handleArray, char* fmtpath)
{
	char path[MAX_PATH_LEN];
	char oldnumstr[32];
	char numstr[32];
	int count;
	ZIDoubleData value;
	int ctrlHandle;
	
	int activeControl = GetActiveCtrl(panel);
	
	int controlArray = GetCtrlArrayFromResourceID(panel, handleArray);
	GetNumCtrlArrayItems(controlArray, &count);
	for(int i = 0;i < count;i++) {
		sprintf(path, fmtpath, zurich->connDef->device, i);
		ziAPIGetValueD(zurich->conn, path, &value);
		ctrlHandle = GetCtrlArrayItem(controlArray, i);
		formatNumberScientific(numstr, (double)value);
		GetCtrlVal(panel, ctrlHandle, oldnumstr);
		if (activeControl != ctrlHandle) {
			SetCtrlVal(panel, ctrlHandle, numstr);
		}
	}
}


void updateControlD(ZurichConn* zurich, int panel, int handleArray, char* fmtpath)
{
	char path[MAX_PATH_LEN];
	int count;
	ZIDoubleData value;
	double oldValue;
	int ctrlHandle;
	
	int controlArray = GetCtrlArrayFromResourceID(panel, handleArray);
	GetNumCtrlArrayItems(controlArray, &count);
	for(int i = 0;i < count;i++) {
		sprintf(path, fmtpath, zurich->connDef->device, i);
		ziAPIGetValueD(zurich->conn, path, &value);
		ctrlHandle = GetCtrlArrayItem(controlArray, i);
		GetCtrlVal(panel, ctrlHandle, &oldValue);
		if (oldValue != (double)value) {
			SetCtrlVal(panel, ctrlHandle, (double)value);
		}
	}
}


void updateControlI(ZurichConn* zurich, int panel, int handleArray, char* fmtpath)
{
	char path[MAX_PATH_LEN];
	int count;
	ZIIntegerData value;
	int oldValue;
	int ctrlHandle;
	
	int controlArray = GetCtrlArrayFromResourceID(panel, handleArray);
	GetNumCtrlArrayItems(controlArray, &count);
	for(int i = 0;i < count;i++) {
		sprintf(path, fmtpath, zurich->connDef->device, i);
		ziAPIGetValueI(zurich->conn, path, &value);
		ctrlHandle = GetCtrlArrayItem(controlArray, i);
		GetCtrlVal(panel, ctrlHandle, &oldValue);
		if (oldValue != (int)value) {
			SetCtrlVal(panel, ctrlHandle, (int)value);
		}
	}
}


// Called in an Async Timer to update the displayed UI values
int CVICALLBACK updateZurichUIControls(int reserved, int timerId, int event, void *callbackData, int eventData1, int eventData2)
{
	ZMeasure* zmeasure = callbackData;
	ZurichConn* zurich = zmeasure->activeConn->conn;
	
	int panel = zmeasure->panels->main;
	char path[MAX_PATH_LEN];
	int controlArray;
	int ctrlHandle;
	int count;
	
	// Get osc frequencies
	updateControlStrD(zurich, panel, FREQ, "/%s/oscs/%d/freq");
	
	// Get each demod's oscillator
	updateControlI(zurich, panel, OSCS, "/%s/demods/%d/oscselect");
	
	// Get each demod's harmonic
	updateControlI(zurich, panel, HARM, "/%s/demods/%d/harmonic");
	
	// Get each demod's phase
	updateControlD(zurich, panel, PHASE, "/%s/demods/%d/phaseshift");
	
	
	// Get each demod's input signal
	updateControlI(zurich, panel, SIG, "/%s/demods/%d/adcselect");
	
	// Get each demod's filter order
	updateControlI(zurich, panel, ORDER, "/%s/demods/%d/order");
	
	return 0;
}

void autophase(int panel, int control)
{
	// Get UI selected connection (we can't use zmeasure->activeConn since its in a different thread)
	int fakeptr;
	GetCtrlVal(panel, MAINP_CONNECTIONS, &fakeptr);
	ZurichConn* zurich = (ZurichConn*)fakeptr;
	
	char path[MAX_PATH_LEN];
	int index;
	int arrayHandle = GetCtrlArrayFromResourceID(panel, AUTOPHASE);
	GetCtrlArrayIndex(arrayHandle, panel, control, &index);
	if (index != -1) {
		sprintf(path, "/%s/demods/%d/phaseadjust", zurich->connDef->device, index);
		ziAPISetValueI(zurich->conn, path, 1);
		return;
	}
}

// Returns 0 if the number was successfully parsed
// Returns 1 if the number couldn't be determined
int readNumberScientific(char* instr, double* value)
{
	size_t len = strlen(instr);
	
	// Get a new string to work with;
	char* str = malloc((len+1) * sizeof(char));
	for(int i = 0;i < len+1;i++) {
		str[i] = 0;
	}
	
	// Remove all spaces
	char c;
	int index = 0;
	for(int i = 0;i < len+1;i++) {
		c = instr[i];
		if (c != ' ') {
			str[index] = c;
			index += 1;
		}
	}
	len = strlen(str);
	
	// Determine if positive or negative
	int sign;
	int startIndex = 0;
	c = str[0];
	if ((c >= '0' && c <= '9') || c == '.') {
		sign = 1;
		startIndex = 0;
	} else if (c == '+') {
		sign = 1;
		startIndex = 1;
	} else if (c == '-') {
		sign = -1;
		startIndex = 1;
	} else {
		free(str);
		return 1;
	}
	
	// Find end of numeric portion:
	int endIndex = len;
	for(int i = startIndex;i < len;i++) {
		c = str[i];
		if ((c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E' || c == '-' || c == '+') {
			endIndex = i;
		}
	}
	endIndex += 1;
	
	// Determine if we are done
	int multiplier = 1;	
	if (endIndex < len) {
		// Determine multiplier
		c = str[endIndex];
		switch (c) {
			case 'G':
				multiplier = 1e9;
				break;
			case 'M':
				multiplier = 1e6;
				break;
			case 'k':
				multiplier = 1e3;
				break;
			case 'm':
				multiplier = 1e-3;
				break;
			case 'u':
				multiplier = 1e-6;
				break;
			case 'n':
				multiplier = 1e-9;
				break;
			case 'p':
				multiplier = 1e-12;
				break;
			default:
				free(str);
				return 1;
		}
	}
	
	// Too many characters
	if (endIndex > len) {
		free(str);
		return 1;
	}
	
	// Calculate value
	str[endIndex] = 0;
	*value = atof(str); 
	*value = *value * multiplier; 
	
	
	free(str);
	return 0;
}

void formatNumberScientific(char* str, double value)
{
	if (value >= 1e9) {
		sprintf(str, "%3.6f G", value/1e9);	
	} else if (value >= 1e6) {
		sprintf(str, "%3.6f M", value/1e6);
	} else if (value >= 1e3) {
		sprintf(str, "%3.6f k", value/1e3);
	} else if (value >= 1) {
		sprintf(str, "%3.6f", value);
	} else if (value >= 1e-3) {
		sprintf(str, "%3.6f m", value*1e3);
	} else if (value >= 1e-3) {
		sprintf(str, "%3.6f u", value*1e6);
	} else if (value >= 1e-3) {
		sprintf(str, "%3.6f n", value*1e9);
	} else if (value >= 1e-3) {
		sprintf(str, "%3.6f p", value*1e12);
	} else {
		sprintf(str, "%3.6e", value);
	}
}

void displayNodeData(int panel, struct MeasurementLegacy* MeasurementLegacy)
{
	int treeIndex;
	char path[128];
	int fakenode = 0;
	
	GetActiveTreeItem(panel, ZNODESP_SETTINGTREE, &treeIndex);
	GetTreeItemAttribute(panel, ZNODESP_SETTINGTREE, treeIndex, ATTR_CTRL_VAL, &fakenode);
	
	struct ZurichNode* node = (struct ZurichNode*) fakenode;
	getZurichTreePath(node, path);
	
	printf("%s\n", path);
}

int CVICALLBACK nodeTree_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	struct MeasurementLegacy* MeasurementLegacy;
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &MeasurementLegacy);
			displayNodeData(panel, MeasurementLegacy);
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
					int treeSelectedIndex, treeSelectedRoot;
					int fakenode;
					struct ZurichNode* node;
					struct ZurichData* zurich;

					// Get the selected item index
					GetActiveTreeItem(panel, ZNODESP_SETTINGTREE, &treeSelectedIndex);
					
					// Get the root node for the selected item
					GetTreeItem (panel, ZNODESP_SETTINGTREE, VAL_ANCESTOR, treeSelectedIndex, VAL_LAST, VAL_NEXT_PLUS_SELF, 0, &treeSelectedRoot);
					
					// Value will be -1 if the node is the root
					if(treeSelectedRoot == -1) {
						treeSelectedRoot = treeSelectedIndex;
					}
					
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

