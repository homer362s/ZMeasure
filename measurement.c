// Contains functions relating to physically taking a measurement

#include <ansi_c.h> 
#include <userint.h>
#include <utility.h>  

#include "fixedziAPI.h"
#include "meastypes.h"

#include "measurement.h"


// Try to connect to the UHFLI
static struct ZurichData* connectToZurich(char* address, uint16_t port, char* device)
{
	// Create new ZurichData struct
	struct ZurichData* zurich = malloc(sizeof(struct ZurichData));
	zurich->path = malloc(MAX_NODE_PATH * sizeof(char));
	zurich->path[0] = 0;
	zurich->device[0] = 0;
	zurich->conn = 0;
	zurich->event = ziAPIAllocateEventEx();
	
	// Initialize ZIConnection.
	zurich->retVal = ziAPIInit(&zurich->conn);
	if (zurich->retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(zurich->retVal, &zurich->errBuffer, NULL);
		fprintf(stderr, "Can't init Connection: %s\n", zurich->errBuffer);
		return 0;
	}
	
	
	zurich->retVal = ziAPIConnectEx(zurich->conn, address, port, ZI_API_VERSION_6, NULL);
	if (zurich->retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(zurich->retVal, &zurich->errBuffer, NULL);
		fprintf(stderr, "Error, can't connect to the Data Server: `%s`.\n", zurich->errBuffer);
		return 0;
	} 
	
	return zurich;
}

static void disconnectFromZurich(struct ZurichData* zurich)
{
	// Delete specified connection
	ziAPIUnSubscribe(zurich->conn, "*"); 
	ziAPIDeallocateEventEx(zurich->event);
	ziAPIDisconnect(zurich->conn);
	ziAPIDestroy(zurich->conn);
	free(zurich->path);
	free(zurich); 
}


// Thread callback to start a new thread to run the measurement
int CVICALLBACK MeasurementThread(struct MeasDef* measDef)
{
	// We need to establish a new connection to the zurich for this thread
	struct ZurichData* zurich = connectToZurich(measDef->ziaddr, measDef->ziport, measDef->zidev);
	
	// Do some test measurements for now
	ZIDemodSample demodSample;
	for(int i = 0;i < 10;i++) {
		ziAPIGetDemodSample(zurich->conn, "/dev2086/demods/0/sample", &demodSample);
		printf("x: %e\t\ty: %e\n", demodSample.x, demodSample.y);
		Delay(1);
	}
	
	
	// Close connection
	disconnectFromZurich(zurich);
	
	// Destroy the measdef now that we are done with it.
	// TODO: make sure this is done properly
	free(measDef);
	return 0;
}


void startMeasurementThread()
{
	// Start the measurement thread
	CmtThreadFunctionID threadID;
	struct MeasDef* measDef = malloc(sizeof(struct MeasDef));
	measDef->measnodes = 0;
	measDef->ziaddr = "newsix.ee.nd.edu";
	measDef->ziport = 8004;
	measDef->zidev = "dev2086";
	CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, MeasurementThread, measDef, &threadID);
}

// UI callback to start, stop, or pause the measurement
int CVICALLBACK startstop_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			startMeasurementThread();
			break;
	}
	return 0;
}

