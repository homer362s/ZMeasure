#include "meastypes.h"

/***** Functions to create and delete structs *****/
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

void deleteZurichConnDef(ZurichConnDef* connDef)
{
	free(connDef->address);
	free(connDef->device);
	free(connDef);
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


Measurement* newMeasurement(ZMeasure* zmeasure)
{
	Measurement* measurement = malloc(sizeof(Measurement));
	measurement->zmeasure = zmeasure;
	measurement->nSteps = 0;  
	for(int i = 0;i < MAX_MEAS_STEPS;i++) {
		measurement->steps[i] = 0;
	}
	measurement->panel = 0;
	
	measurement->requestAction = MEAS_ACTION_NONE;
	
	CmtNewLock(0, 0, &measurement->threadLock);
	
	measurement->measTree = newTree(5);
	measurement->measTree->data = malloc(1 * sizeof(char));
	strcpy(measurement->measTree->data, "");
	
	return measurement;
}

void deleteMeasurement(Measurement* measurement)
{
	for(int i = 0;i < measurement->nSteps;i++) {
		deleteMeasStep(measurement->steps[i]);
	}
	
	CmtDiscardLock(measurement->threadLock);
	
	free(measurement);
}

MeasStep* newMeasStep(Measurement* measurement)
{
	MeasStep* measStep = malloc(sizeof(MeasStep));
	measStep->delay = 0;
	measStep->nPoints = 0;
	measStep->parent = measurement;
	measStep->nVars = 0;
	for(int i = 0;i < MAX_MEAS_VARS;i++) {
		measStep->vars[i] = 0;
	}
	
	return measStep;
}

void deleteMeasStep(MeasStep* measStep)
{
	for(int i = 0;i < measStep->nVars;i++) {
		deleteMeasVar(measStep->vars[i]);
	}
	
	free(measStep);
}

MeasVar* newMeasVar(MeasStep* measStep)
{
	MeasVar* measVar = malloc(sizeof(MeasVar));
	measVar->conn = 0;
	measVar->parent=measStep;
	measVar->path = 0;
	measVar->sweepType = 0;
	measVar->values = 0;
	
	return measVar;
}

// Note: This doesn't free the ZurichConn under measVar->conn
// This is because it is expected that multiple variables will
// use the same connection to access their data during the
// measurement
void deleteMeasVar(MeasVar* measVar)
{
	free(measVar->values);
	free(measVar->path);
	free(measVar);
}


// Get the index of a ZurichConn in a ZMeasure struct
// Returns -1 if not found
int getZurichConnIndex(ZMeasure* zmeasure, ZurichConn* zurich, size_t length)
{
	for(int i = 0;i < length;i++) {
		if (zmeasure->connections[i] == zurich) {
			return i;
		}
	}
	return -1;
}

// Adds a new ZurichConn to the ZMeasure
// Returns 0 on success, 1 on failure (at connection limit)
int addZurichConnToZMeasure(ZMeasure* zmeasure, ZurichConn* zurich)
{
	if (zmeasure->connCount >= MAX_DEVICE_CONNECTIONS) {
		return 1;
	}
	
	zmeasure->connections[zmeasure->connCount] = zurich;
	zmeasure->connCount += 1;
	
	return 0;
}

// Removes a ZurichConn from the ZMeasure
// Returns 0 on success, 1 on failure (ZurichConn not found)
int removeZurichConnFromZMeasure(ZMeasure* zmeasure, ZurichConn* zurich)
{
	int index = getZurichConnIndex(zmeasure, zurich, zmeasure->connCount);
	if (index == -1) {
		return 1;
	}
	
	// Shift following elements forward
	for(int i = index+1;i < zmeasure->connCount;i++) {
		zmeasure->connections[i-1] = zmeasure->connections[i];
	}
	
	// Set end item to zero
	zmeasure->connections[zmeasure->connCount-1] = 0;
	
	// Update connection count
	zmeasure->connCount -= 1;
	
	return 0;
}

