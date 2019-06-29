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
	zmeasure->activeConn->timer = NewAsyncTimer(1.0, -1, 0, updateZurichUIControls, zmeasure);
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


// Called in an Async Timer to update the displayed UI values
int CVICALLBACK updateZurichUIControls(int reserved, int timerId, int event, void *callbackData, int eventData1, int eventData2)
{
	ZMeasure* zmeasure = callbackData;
	ZurichConn* zurich = zmeasure->activeConn->conn;
	
	int panel = zmeasure->panels->main;
	char path[MAX_PATH_LEN];
	char numstr[32];
	int controlArray;
	int ctrlHandle;
	int count;
	
	// Get osc frequencies
	double freq;
	controlArray = GetCtrlArrayFromResourceID(panel, FREQ);
	GetNumCtrlArrayItems(controlArray, &count);
	for(int i = 0;i < count;i++) {
		sprintf(path, "/%s/oscs/%d/freq", zurich->connDef->device, i);
		ziAPIGetValueD(zurich->conn, path, &freq);
		ctrlHandle = GetCtrlArrayItem(controlArray, i);
		formatNumberScientific(numstr, freq);
		SetCtrlVal(panel, ctrlHandle, numstr);
	}
	
	// Get each demod's oscillator
	ZIIntegerData osc; 
	controlArray = GetCtrlArrayFromResourceID(panel, OSCS);
	GetNumCtrlArrayItems(controlArray, &count);
	for(int i = 0;i < count;i++) {
		sprintf(path, "/%s/demods/%d/oscselect", zurich->connDef->device, i);
		ziAPIGetValueI(zurich->conn, path, &osc);
		ctrlHandle = GetCtrlArrayItem(controlArray, i);
		SetCtrlVal(panel, ctrlHandle, (int)osc);
	}
	
	// Get each demod's harmonic
	ZIIntegerData harm;
	controlArray = GetCtrlArrayFromResourceID(panel, HARM);
	GetNumCtrlArrayItems(controlArray, &count);
	for(int i = 0;i < count;i++) {
		sprintf(path, "/%s/demods/%d/harmonic", zurich->connDef->device, i);
		ziAPIGetValueI(zurich->conn, path, &harm);
		ctrlHandle = GetCtrlArrayItem(controlArray, i);
		SetCtrlVal(panel, ctrlHandle, (int)harm);
	}
	
	// Get each demod's phase
	ZIDoubleData phase;
	controlArray = GetCtrlArrayFromResourceID(panel, PHASE);
	GetNumCtrlArrayItems(controlArray, &count);
	for(int i = 0;i < count;i++) {
		sprintf(path, "/%s/demods/%d/phaseshift", zurich->connDef->device, i);
		ziAPIGetValueI(zurich->conn, path, &phase);
		ctrlHandle = GetCtrlArrayItem(controlArray, i);
		SetCtrlVal(panel, ctrlHandle, phase);
	}
	
	// Get each demod's input signal
	ZIIntegerData sig;
	controlArray = GetCtrlArrayFromResourceID(panel, SIG);
	GetNumCtrlArrayItems(controlArray, &count);
	for(int i = 0;i < count;i++) {
		sprintf(path, "/%s/demods/%d/adcselect", zurich->connDef->device, i);
		ziAPIGetValueI(zurich->conn, path, &sig);
		ctrlHandle = GetCtrlArrayItem(controlArray, i);
		SetCtrlVal(panel, ctrlHandle, (int)sig);
	}
	
	// Get each demod's filter order
	ZIIntegerData order;
	controlArray = GetCtrlArrayFromResourceID(panel, ORDER);
	GetNumCtrlArrayItems(controlArray, &count);
	for(int i = 0;i < count;i++) {
		sprintf(path, "/%s/demods/%d/order", zurich->connDef->device, i);
		ziAPIGetValueI(zurich->conn, path, &order);
		ctrlHandle = GetCtrlArrayItem(controlArray, i);
		SetCtrlVal(panel, ctrlHandle, (int)order);
	}
	
	return 0;
}

void formatNumberScientific(char* str, double value)
{
	if (value >= 1e6) {
		sprintf(str, "%3.3f M", value/1e6);
	} else if (value >= 1e3) {
		sprintf(str, "%3.3f k", value/1e3);
	} else if (value < 1) {
		sprintf(str, "%3.3e", value);
	} else {
		sprintf(str, "%3.3f", value);
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

