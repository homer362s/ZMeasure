// Includes struct definitions that will be used throughout the program

#ifndef __meastypes_H__
#define __meastypes_H__

// Required headers for this file
#include "fixedziAPI.h"

#define MAX_DEV_NAME_LENGTH 8
#define MAX_DEVICE_CONNECTIONS 16
#define MAX_NODE_PATH 128
#define MAX_MEAS_STEPS 10
#define MAX_MEAS_VARS 10

/***** New types *****/


// Typedefs for the following structs
// If we don't pre-typedef we can't have the circular references we want
typedef struct ZMeasure ZMeasure;
typedef struct ActiveConn ActiveConn;
typedef struct Measurement Measurement;
typedef struct PrimaryPanels PrimaryPanels;
typedef struct ZurichConn ZurichConn;
typedef struct ZurichConnDef ZurichConnDef;
typedef struct MeasStep MeasStep; 
typedef struct MeasVar MeasVar; 
typedef struct LinearSweep LinearSweep;
typedef struct RelativeSweep RelativeSweep;


 // Stores everything there is to know about the entire program
struct ZMeasure {
	PrimaryPanels* panels;								// Panels for all of the primary functions of the UI
	ZurichConn* connections[MAX_DEVICE_CONNECTIONS];	// Array of connections to zurich devices
	uint32_t connCount;									// Number of active connections to zurich devices
	ActiveConn* activeConn;								// Connection displayed in the main UI window
};

struct ActiveConn {
	int timer;					// Async timer reference
	ZurichConn* conn;			// Zurich connection used in the Async timer thread
};


// A definition of a MeasurementLegacy
// A MeasurementLegacy is made up of one ore more MeasurementLegacy steps
// Each step is made up of zero or more MeasurementLegacy variables
// (zero for repeated MeasurementLegacys for averaging)
struct Measurement {
	uint32_t nSteps;					// Number of MeasurementLegacy steps
	MeasStep* steps[MAX_MEAS_STEPS];	// Array of each MeasurementLegacy step	
};


// Stores handles to all of the "persistent" panels
// That is those main top level panels that there should only ever be a single
// copy of. This way we don't need to constantly create and destroy the panels
// and simplifies raising the existing panel if it is "opened" while already
// open.
struct PrimaryPanels {
	int main;			// Primary program window
	int about;			// About window
	int newZConn;		// New connection window
};


// Data associated with a connection to a zurich device
struct ZurichConn {
	ZIConnection conn;					// ziAPI connection
	ZIResult_enum retVal;				// Return value from most recently called ziAPI function
	ZIEvent* event;						// ziAPI event buffer for receiving streaming data
	char* errBuffer;					// Buffer to store the most recent error string
	ZurichConnDef* connDef;				// Server, port, and device information
};


// All the information necessary to establish a connection to a zurich instruments device
struct ZurichConnDef {
	char* address;						// Data server address
	uint16_t port;						// Data server port
	char* device;						// Name of zurich device
};


// Information about a MeasurementLegacy step.
struct MeasStep {
	uint32_t nPoints;				// Number of points in this MeasurementLegacy step
	MeasVar* vars[MAX_MEAS_VARS];	// Array of variables to modify at this MeasurementLegacy step
	float delay;					// Time delay, in seconds,between each MeasurementLegacy at this step
};


// This variable is a regular linear sweep
struct LinearSweep {
	double start;		// Starting value
	double step;		// Increment value
};

// This variable is relative to another variable
struct RelativeSweep {
	MeasVar* ref;		// Variable that this variable is relative to
	double coeff;		// Multiplicative coefficient. This variable's value is coeff * value of ref
};

// Information about a MeasurementLegacy variable
enum SweepType{SWEEP_TYPE_SINGLE, SWEEP_TYPE_DUAL, SWEEP_TYPE_RELATIVE};   
struct MeasVar {
	ZurichConn* conn;			// Connection to use for this variable
	char* path;					// Path to the node to set at each step
	enum SweepType sweepType;	// Which type of sweep is contained in the struct
	union {
		LinearSweep singleSweep;
		LinearSweep dualSweep;
		RelativeSweep relativeSweep;
	};
	double* values;				// Array of precomputed values for the sweep
};





/***** Legacy types *****/
struct PanelHandles {
	int main;
	int about;
	int zconn;
	int znodes;
	int sweep;
	int output;	
	int measvars;
};

struct ZurichData {
	ZIResult_enum retVal;
	ZIConnection conn;
	ZIEvent* event;
	char device[MAX_DEV_NAME_LENGTH];		// Device name
	char* errBuffer;					// Holds the last error from the device
	char* path;							// Temporary variable to hold the last path built
	struct ZurichNode* tree;			// Node tree for all nodes on the device
};

struct MeasurementLegacy {
	struct PanelHandles* panels;
	int connectionCount;
	struct ZurichData* zurich[MAX_DEVICE_CONNECTIONS];	// Array of connections to zurich devices
	struct MeasurementLegacyNodes* measNodes;					// Paths to measure
};

// Definition of a MeasurementLegacy to make
// Passed to the MeasurementLegacy thread to initiate a MeasurementLegacy
struct MeasDef {
	char* ziaddr;
	uint16_t ziport;
	char* zidev;
	char** measnodes;
	//CmtTSQHandle* toMeasHandle;
	//CmtTSQHandle* fromMeasHandle;
};

struct ThreadMessage {
	
};
	

#endif  /* ndef __meastypes_H__ */
