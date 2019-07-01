// Contains functions relating to physically taking a measurement
#include "measurement.h"

#include <userint.h>   
#include <utility.h>  

#include "measurement_u.h"
#include "zmeasure.h"

#define MEAS_STOPPED 0
#define MEAS_RUNNING 1
#define MEAS_PAUSED 2

/***** Private function prototypes *****/
static void setUIState(int panel, int state);
static void startMeasurement(Measurement* measurement);
static void pauseMeasurement(Measurement* measurement);
static void stopMeasurement(Measurement* measurement);

int CVICALLBACK measurementThread (void* measurement);


/***** Functions for running and controlling the measurement *****/
static void startMeasurement(Measurement* measurement)
{
	// Start the measurement thread. Hold the thread lock for measurement->threadID
	CmtGetLock(measurement->threadLock);
	CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, measurementThread, measurement, &measurement->threadID);
	CmtReleaseLock(measurement->threadLock);
	
	// Dim/undim appropriate ui elements
	setUIState(measurement->panel, MEAS_RUNNING);
}

static void pauseMeasurement(Measurement* measurement)
{
	
	setUIState(measurement->panel, MEAS_PAUSED);
}

static void stopMeasurement(Measurement* measurement)
{
	
	setUIState(measurement->panel, MEAS_STOPPED);
}


static void setUIState(int panel, int state)
{
	int ctrlArray = GetCtrlArrayFromResourceID(panel, SETUP);
	switch (state) {
		case MEAS_RUNNING:
			SetCtrlArrayAttribute(ctrlArray, ATTR_DIMMED, 1);
			break;
		case MEAS_PAUSED:
		case MEAS_STOPPED:
			SetCtrlArrayAttribute(ctrlArray, ATTR_DIMMED, 0);
			SetCtrlAttribute(panel, MEASP_START, ATTR_DIMMED, 0);
			break;
	}
	
	switch (state) {
		case MEAS_RUNNING:
		case MEAS_STOPPED:
			SetCtrlAttribute(panel, MEASP_PAUSE, ATTR_LABEL_TEXT, "Pause");
			break;
		case MEAS_PAUSED:
			SetCtrlAttribute(panel, MEASP_PAUSE, ATTR_LABEL_TEXT, "Resume");
			break;
			
	}
	
	switch (state) {
		case MEAS_RUNNING:
		case MEAS_PAUSED: 
			SetCtrlAttribute(panel, MEASP_PAUSE, ATTR_DIMMED, 0);
			SetCtrlAttribute(panel, MEASP_STOP, ATTR_DIMMED, 0);
			SetCtrlAttribute(panel, MEASP_START, ATTR_DIMMED, 1);
			break;
		case MEAS_STOPPED:
			SetCtrlAttribute(panel, MEASP_PAUSE, ATTR_DIMMED, 1);
			SetCtrlAttribute(panel, MEASP_STOP, ATTR_DIMMED, 1);
			SetCtrlAttribute(panel, MEASP_START, ATTR_DIMMED, 0);
			break;
	}
}


// Measurement thread
int CVICALLBACK measurementThread (void* functionData)
{
	Measurement* measurement = (Measurement*) functionData;
	
	// Delay for testing purposes. Remove later
	Delay(1);
	
	// Establish necessary ZI connections
	
	// Set up the inner sweeper module
	
	// Handle necessary loops for each measurement step
	
	// Set UI back to stopped state. Get the thread lock to safely
	// handle the measurement->panel variable
	CmtGetLock(measurement->threadLock);
	if (measurement->panel) {
		setUIState(measurement->panel, MEAS_STOPPED);
	}
	CmtReleaseLock(measurement->threadLock);
	
	return 0;
}




/***** Functions for allocating and freeing memory *****/

// Raise (or create if necessary) the settings panel for this measurement
void raiseMeasurementPanel(Measurement* measurement)
{
	// We need the thread lock for this entire function to
	// safely use the measurement->panel variable
	CmtGetLock(measurement->threadLock);
	
	// If the panel doesn't exist, make one
	if (!measurement->panel) {
		ZMeasure* zmeasure = measurement->zmeasure;
		measurement->panel = LoadPanel(zmeasure->panels->main, "measurement_u.uir", MEASP);
		SetPanelAttribute(measurement->panel, ATTR_CALLBACK_DATA, measurement);
	}
	
	// Load into the panel all the measurement settings
	// TODO: implement loading measurement settings
	
	// Display the panel
	DisplayPanel(measurement->panel);
	
	// Return the thread lock
	CmtReleaseLock(measurement->threadLock);
}

void createNewMeasurement(ZMeasure* zmeasure)
{
	// Create Measurement struct
	Measurement* measurement = newMeasurement(zmeasure);
	
	// Create a new measurement settings panel
	measurement->panel = LoadPanel(zmeasure->panels->main, "measurement_u.uir", MEASP);
	
	// Store the measurement pointer in the panel's callback data
	SetPanelAttribute(measurement->panel, ATTR_CALLBACK_DATA, measurement);
	
	// Update main window tree. Store Measurement* measurement as an int since void* isn't an option
	InsertTreeItem(zmeasure->panels->main, MAINP_MEASUREMENTS, VAL_SIBLING, 0, VAL_LAST, "Untitled Measurement", NULL, NULL, (int)measurement);
	
	// Display the panel
	raiseMeasurementPanel(measurement);
}




/***** Callback Functions *****/
int CVICALLBACK measpanel_CB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_KEYPRESS)
		switch(eventData1) {
			case VAL_ESC_VKEY:
				Measurement* measurement = callbackData;
				DiscardPanel(measurement->panel);
				measurement->panel = 0;
	}
	return 0;
}


int CVICALLBACK startstop_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT) {
		Measurement* measurement;
		GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measurement);
		switch (control) {
			case MEASP_START:
				startMeasurement(measurement);
				break;
			case MEASP_PAUSE:
				pauseMeasurement(measurement);
				break;
			case MEASP_STOP:
				stopMeasurement(measurement);
				break;
		}
	}
	return 0;
}

