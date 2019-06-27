// Includes struct definitions that will be used throughout the program

#ifndef __meastypes_H__
#define __meastypes_H__

#define MAX_DEVICE_LENGTH 8
#define MAX_DEVICE_CONNECTIONS 16
#define MAX_NODE_PATH 128
#define MAX_MEAS_STEPS 10
#define MAX_MEAS_VARS 10

/***** New types *****/


// Typedefs for the following structs
// If we don't pre-typedef we can't have the circular references we want
typedef struct ZMeasure ZMeasure;
typedef struct Measurement2 Measurement2;
typedef struct PrimaryPanels PrimaryPanels;
typedef struct ZurichConn ZurichConn;
typedef struct MeasStep MeasStep; 
typedef struct MeasVar MeasVar; 
typedef struct LinearSweep LinearSweep;
typedef struct RelativeSweep RelativeSweep;


 // Stores everything there is to know about the entire program
struct ZMeasure {
	PrimaryPanels* panels;								// Panels for all of the primary functions of the UI
	ZurichConn* connections[MAX_DEVICE_CONNECTIONS];	// Array of connections to zurich devices
	uint32_t connCount;									// Number of active connections to zurich devices
};


// A definition of a measurement
// A measurement is made up of one ore more measurement steps
// Each step is made up of zero or more measurement variables
// (zero for repeated measurements for averaging)
struct Measurement2 {
	uint32_t nSteps;					// Number of measurement steps
	MeasStep* steps[MAX_MEAS_STEPS];	// Array of each measurement step	
};


// Stores handles to all of the "persistent" panels
// That is those main top level panels that there should only ever be a single
// copy of. This way we don't need to constantly create and destroy the panels
// and simplifies raising the existing panel if it is "opened" while already
// open.
struct PrimaryPanels {
	int main;			// Primary program window
	int about;			// About window
};


// Data associated with a connection to a zurich device
struct ZurichConn {
	ZIConnection conn;					// ziAPI connection
	ZIResult_enum retVal;				// Return value from most recently called ziAPI function
	ZIEvent* event;						// ziAPI event buffer for receiving streaming data
	char* errBuffer;					// Buffer to store the most recent error string
	char device[MAX_DEVICE_LENGTH];		// Device name
};


// Information about a measurement step.
struct MeasStep {
	uint32_t nPoints;				// Number of points in this measurement step
	MeasVar* vars[MAX_MEAS_VARS];	// Array of variables to modify at this measurement step
	float delay;					// Time delay, in seconds,between each measurement at this step
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

// Information about a measurement variable
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
	char device[MAX_DEVICE_LENGTH];		// Device name
	char* errBuffer;					// Holds the last error from the device
	char* path;							// Temporary variable to hold the last path built
	struct ZurichNode* tree;			// Node tree for all nodes on the device
};

struct Measurement {
	struct PanelHandles* panels;
	int connectionCount;
	struct ZurichData* zurich[MAX_DEVICE_CONNECTIONS];	// Array of connections to zurich devices
	struct MeasurementNodes* measNodes;					// Paths to measure
};

// Definition of a measurement to make
// Passed to the measurement thread to initiate a measurement
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
