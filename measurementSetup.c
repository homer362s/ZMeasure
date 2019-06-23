#include <ansi_c.h>
#include <userint.h>

#include "fixedziAPI.h"

#include "ZMeasure.h"
#include "zurichNodeTree.h"
#include "measurementSetup.h"
#include "ziUtility.h"


// Create main measurement struct
struct Measurement* allocateMeasurement()
{
	struct Measurement* measurement = malloc(sizeof(struct Measurement));  
	measurement->connectionCount = 0;
	measurement->panels = malloc(sizeof(struct PanelHandles));

	return measurement; 
}

// Free the entire measurement struct.
void deleteMeasurement(struct Measurement* measurement)
{
	free(measurement->panels); measurement->panels = 0;
	for(int index = measurement->connectionCount - 1;index >= 0;index--) {
		deleteZurichConnection(measurement, index);
	}
	free(measurement); measurement = 0;
}


// Try to connect to the UHFLI
void connectToZurich(struct Measurement* measurement)
{
	struct ZurichData* zurich = newZurichConnection(measurement);
	int panel = measurement->panels->zconn;
	
	// Read address from UI
	uint16_t port;
	char address[64];
	
	GetCtrlVal(panel, ZCONNP_PORT, &port);
	GetCtrlVal(panel, ZCONNP_SERVER, address);
	GetCtrlVal(panel, ZCONNP_DEVICE, zurich->device);
	
	// Initialize ZIConnection.
	zurich->retVal = ziAPIInit(&zurich->conn);
	if (zurich->retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(zurich->retVal, &zurich->errBuffer, NULL);
		fprintf(stderr, "Can't init Connection: %s\n", zurich->errBuffer);
		return;
	}
	
	
	zurich->retVal = ziAPIConnectEx(zurich->conn, address, port, ZI_API_VERSION_6, NULL);
	if (zurich->retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(zurich->retVal, &zurich->errBuffer, NULL);
		fprintf(stderr, "Error, can't connect to the Data Server: `%s`.\n", zurich->errBuffer);
		return;
	} 
	
	populateTree(measurement, ZNODESP_SETTINGTREE, ZI_LIST_NODES_NONE);
}

void disconnectFromZurich(struct Measurement* measurement, int index)
{
	deleteZurichConnection(measurement, index);
	
	// TODO: add UI updates
}


// Create a new Zurich connection struct and add it to the measurement
struct ZurichData* newZurichConnection(struct Measurement* measurement)
{
	int newIndex = measurement->connectionCount;
	
	// Don't create the connection of we are at the limit
	if (newIndex >= MAX_DEVICE_CONNECTIONS) {
		printf("Can't create new connection. Connection limit (%d) reached.", MAX_DEVICE_CONNECTIONS);
		return 0;
	}
	
	measurement->connectionCount = newIndex + 1;
	struct ZurichData* zurich = malloc(sizeof(struct ZurichData));
	zurich->path = malloc(128 * sizeof(char));
	zurich->path[0] = 0;
	zurich->device[0] = 0;
	zurich->conn = 0;
	zurich->event = ziAPIAllocateEventEx();
	
	measurement->zurich[newIndex] = zurich;
	
	return zurich;
}

// Free memory for a specified connection
void deleteZurichConnection(struct Measurement* measurement, int index)
{
	struct ZurichData* zurich = measurement->zurich[index];
	
	// Delete specified connection
	ziAPIUnSubscribe(zurich->conn, "*"); 
	ziAPIDeallocateEventEx(zurich->event);
	ziAPIDisconnect(zurich->conn);
	ziAPIDestroy(zurich->conn);
	free(zurich->path);
	free(zurich); 
	
	measurement->zurich[index] = 0;
	
	// Shift later connections forward to fill the gap
	for(int i = index+1;i < measurement->connectionCount;i++) {
		measurement->zurich[i-1] = measurement->zurich[i];
	}
	measurement->zurich[measurement->connectionCount - 1] = 0;
	
	// Update connection count
	measurement->connectionCount = measurement->connectionCount - 1;
}


void displayNodeData(int panel, struct Measurement* measurement)
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
	struct Measurement* measurement;
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measurement);
			displayNodeData(panel, measurement);
			break;
	}
	return 0;
}

int CVICALLBACK manageConnections_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	struct Measurement* measurement;
	GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measurement);
	
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control) {
				case ZNODESP_NEWCONNECTION:
					DisplayPanel(measurement->panels->zconn);
					break;
				case ZNODESP_DISCONNECT:
					printf("not implemented\n");
					break;
			}
			break;
	}
	return 0;
}
