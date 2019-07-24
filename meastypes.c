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
		free(zurich);
		return 0;
	}
	
	
	// Connect to data server
	zurich->retVal = ziAPIConnectEx(zurich->conn, connDef->address, connDef->port, ZI_API_VERSION_6, NULL);
	if (zurich->retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(zurich->retVal, &zurich->errBuffer, NULL);
		fprintf(stderr, "Error, can't connect to the Data Server: `%s`.\n", zurich->errBuffer);
		free(zurich);
		return 0;
	} 
	
	
	// Allocate an event struct
	zurich->event = ziAPIAllocateEventEx();
	
	
	// Save the connection definition 
	zurich->connDef = connDef;
	
	// Return the ZurichConn struct
	return zurich;
}

// Delete a ZurichConn struct and everything inside
// This terminates the connection first if one exists
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
	
	char* defaultName = "Untitled";
	size_t length = strlen(defaultName);
	measurement->name = malloc((length+1) * sizeof(char));
	strcpy(measurement->name, defaultName);
	
	return measurement;
}

void deleteMeasurement(Measurement* measurement)
{
	for(uint32_t i = 0;i < measurement->nSteps;i++) {
		deleteMeasStep(measurement->steps[i]);
	}
	
	CmtDiscardLock(measurement->threadLock);
	
	deleteTree(measurement->measTree, free);
	
	free(measurement->name);
	
	free(measurement);
}

// Create a new MeasStep and add it to a Measurement
MeasStep* newMeasStep(Measurement* measurement)
{
	// If we are at the limit, do nothing, return null pointer
	if (measurement->nSteps >= MAX_MEAS_STEPS) {
		return 0;
	}
	
	// Create and initialize measStep
	MeasStep* measStep = malloc(sizeof(MeasStep));
	measStep->delay = 1;
	measStep->nPoints = 101;
	measStep->nVars = 0;
	measStep->sweepType = SWEEP_TYPE_SINGLE;
	for(int i = 0;i < MAX_MEAS_VARS;i++) {
		measStep->vars[i] = 0;
	}
	
	// Add to parent's child list
	measStep->parent = measurement; 
	measurement->steps[measurement->nSteps] = measStep;
	measurement->nSteps += 1;
	
	// Create default name
	const char* sweepTemplate = "Sweep %d";
	const char* stepTemplate = "Step %d";
	char* thisTemplate;
	size_t nSteps = measurement->nSteps;
	if (nSteps == 0) {
		thisTemplate = sweepTemplate;
	} else {
		thisTemplate = stepTemplate;
	}
	
	ssize_t len = snprintf(0, 0, thisTemplate, (int)nSteps);
	measStep->name = malloc((len+1) * sizeof(char));
	sprintf(measStep->name, thisTemplate, (int)nSteps);
	
	return measStep;
}

void deleteMeasStep(MeasStep* measStep)
{
	Measurement* measurement = measStep->parent;
	// Remove from parent
	long int index = getMeasStepIndex(measurement, measStep);
	if (index >= 0) {
		measurement->steps[index] = 0;
		for(size_t i = index+1;i < measurement->nSteps;i++) {
			measurement->steps[i-1] = measurement->steps[i];
		}
		measurement->nSteps -= 1;
		measurement->steps[measurement->nSteps] = 0;
	}
	
	// Delete measVars
	for(uint32_t i = 0;i < measStep->nVars;i++) {
		deleteMeasVar(measStep->vars[i]);
	}
	
	free(measStep->name);
	
	free(measStep);
}

// TODO: Add to the parent's child array and count
MeasVar* newMeasVar(MeasStep* measStep)
{
	// If we are at the variable limit, do nothing, return null pointer
	if (measStep->nVars >= MAX_MEAS_VARS) {
		return 0;
	}
	
	// Create and initialize measVar
	MeasVar* measVar = malloc(sizeof(MeasVar));
	measVar->conn = 0;
	measVar->path = 0;
	measVar->values = 0;
	measVar->varType = VAR_TYPE_INDEPENDENT;
	
	// Add to parent's child list
	measVar->parent=measStep;
	measStep->vars[measStep->nVars] = measVar;
	measStep->nVars += 1;
	
	// Create default name
	const char* thisTemplate = "Var %d";
	size_t nVars = measStep->nVars;
	
	ssize_t len = snprintf(0, 0, thisTemplate, (int)nVars);
	measVar->name = malloc((len+1) * sizeof(char));
	sprintf(measVar->name, thisTemplate, (int)nVars);
	
	return measVar;
}

// Note: This doesn't free the ZurichConn under measVar->conn
// This is because it is expected that multiple variables will
// use the same connection to access their data during the
// measurement
void deleteMeasVar(MeasVar* measVar)
{
	free(measVar->name);
	free(measVar->values);
	free(measVar->path);
	free(measVar);
}


// Get the index of a ZurichConn in a ZMeasure struct
// Returns -1 if not found
size_t getZurichConnIndex(ZMeasure* zmeasure, ZurichConn* zurich)
{
	for(size_t i = 0;i < zmeasure->connCount;i++) {
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
// This function does not free any memory
int removeZurichConnFromZMeasure(ZMeasure* zmeasure, ZurichConn* zurich)
{
	long int index = getZurichConnIndex(zmeasure, zurich);
	if (index == -1) {
		return 1;
	}
	
	// Shift following elements forward
	for(size_t i = index+1;i < zmeasure->connCount;i++) {
		zmeasure->connections[i-1] = zmeasure->connections[i];
	}
	
	// Set end item to zero
	zmeasure->connections[zmeasure->connCount-1] = 0;
	
	// Update connection count
	zmeasure->connCount -= 1;
	
	return 0;
}

// Returns the index of a pointer in an array
long int getMeasStepIndex(Measurement* measurement, MeasStep* measStep)
{
	for(size_t i = 0;i < measurement->nSteps;i++) {
		if (measurement->steps == measStep) {
			return i;
		}
	}
	return -1;
}

// Get the index of a Measurement in a ZMeasure struct
// Returns -1 if not found
long int getMeasurementIndex(ZMeasure* zmeasure, Measurement* measurement)
{
	for(size_t i = 0;i < zmeasure->measurementCount;i++) {
		if (zmeasure->measurements[i] == measurement) {
			return i;
		}
	}
	return -1;
}


// Adds a Weasurement to the ZMeasure
// Returns 0 on success, 1 on failure (at connection limit)
int addMeasurementToZMeasure(ZMeasure* zmeasure, Measurement* measurement)
{
	if (zmeasure->measurementCount >= MAX_MEASUREMENTS) {
		return 1;
	}
	
	zmeasure->measurements[zmeasure->measurementCount] = measurement;
	zmeasure->measurementCount += 1;
	
	return 0;	
}


// Removes a Measurement from the ZMeasure
// Returns 0 on success, 1 on failure (Measurement not found)
// This function does not free any memory
int removeMeasurementFromZMeasure(ZMeasure* zmeasure, Measurement* measurement)
{
	long int index = getMeasurementIndex(zmeasure, measurement);
	if (index == -1) {
		return 1;
	}
	
	// Shift following elements forward
	for(size_t i = index+1;i < zmeasure->measurementCount;i++) {
		zmeasure->measurements[i-1] = zmeasure->measurements[i];
	}
	
	// Set end item to zero
	zmeasure->measurements[zmeasure->measurementCount-1] = 0;
	
	// Update connection count
	zmeasure->measurementCount -= 1;
	
	return 0;
}
