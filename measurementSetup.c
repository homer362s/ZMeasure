#include "measurementSetup.h"

#include <ansi_c.h>
#include <asynctmr.h>
#include <utility.h> 

#include "fixedziAPI.h"
#include "meastypes.h"

#include "ZMeasure.h"
#include "zurichNodeTree.h"
#include "measurement.h"


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
	
	for(int i = 0;i < MAX_MEASUREMENTS;i++) {
		zmeasure->measurements[i] = 0;
	}
	zmeasure->measurementCount = 0;
	
	initMeasNodes();

	return zmeasure;
}

// Free the main system struct
void deleteSystemVars(ZMeasure* zmeasure)
{
	free(zmeasure->activeConn);
	
	// Delete each measurement
	for(size_t i = 0;i < zmeasure->measurementCount;i++) {
		deleteMeasurement(zmeasure->measurements[i]);
	}
	
	// Delete each connection
	for(size_t i = 0;i < zmeasure->connCount;i++) {
		deleteZurichConn(zmeasure->connections[i]);
	}
	
	// Delete remaining variables
	free(zmeasure->panels);
	free(zmeasure);
	
	deleteMeasNodes();
}

// This function is more complicated than I think it should need to be.
// DiscardAsyncTimer() seems to return before the final timer callback
// finishes execution. Since we then proceed to free memory used by that
// callback, we get a "general protection fault" for trying to access it
// This function includes a delay to allow everything to finish executing
// before freeing the memory. This shouldn't be necessary. 
void deleteUITimerThread(ZMeasure* zmeasure)
{
	if (zmeasure->activeConn->conn) {
		// Discard the async timer
		DiscardAsyncTimer(zmeasure->activeConn->timer);
		Delay(0.5);
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
	zmeasure->activeConn->timer = NewAsyncTimer(0.2, -1, 1, updateZurichUIControls, zmeasure);
}




// Returns (in char* name) a formatted string representing the connection name
// char* name must have enough space to hold this string
void getConnName(ZurichConnDef* connDef, char* name)
{
	sprintf(name, "%s  (%s:%d)", connDef->device, connDef->address, connDef->port);
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
	// Only process a timer tick event
	if(event == EVENT_TIMER_TICK) {
		ZMeasure* zmeasure = callbackData;
		ZurichConn* zurich = zmeasure->activeConn->conn;
	
		int panel = zmeasure->panels->main;
	
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
	}
	return 0;
}

void autophase(int panel, int control)
{
	// Get UI selected connection (we can't use zmeasure->activeConn since its in a different thread)
	ZurichConn* zurich;
	GetCtrlVal(panel, MAINP_CONNECTIONS, (int*)(&zurich));
	
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
	for(uint8_t i = 0;i < len+1;i++) {
		str[i] = 0;
	}
	
	// Remove all spaces
	char c;
	int index = 0;
	for(uint8_t i = 0;i < len+1;i++) {
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
	size_t endIndex = len;
	for(size_t i = startIndex;i < len;i++) {
		c = str[i];
		if ((c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E' || c == '-' || c == '+') {
			endIndex = i;
		}
	}
	endIndex += 1;
	
	// Determine if we are done
	double multiplier = 1;	
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
	//int treeIndex;
	//char path[128];
	//int fakenode = 0;
	
	//GetActiveTreeItem(panel, ZNODESP_SETTINGTREE, &treeIndex);
	//GetTreeItemAttribute(panel, ZNODESP_SETTINGTREE, treeIndex, ATTR_CTRL_VAL, &fakenode);
	
	//struct ZurichNode* node = (struct ZurichNode*) fakenode;
	//getZurichTreePath(node, path);
	
	//printf("%s\n", path);
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


int CVICALLBACK manageMeasurements_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	ZMeasure* zmeasure;
	GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &zmeasure);
	
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control) {
				case MAINP_NEWMEAS:
					createMeasurementInUI(zmeasure);
					break;
				case MAINP_DELETEMEAS:
					Measurement* measurement;
					GetCtrlVal(panel, MAINP_MEASUREMENTS, &measurement);
					deleteMeasurementFromUI(measurement);
					break;
			}
			break;
	}
	return 0;
}
