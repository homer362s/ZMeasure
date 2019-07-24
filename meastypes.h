// Includes struct definitions that will be used throughout the program

#ifndef __meastypes_H__
#define __meastypes_H__

// Required headers for this file
#include <ansi_c.h>
#include <utility.h>
#include "fixedziAPI.h"
#include "tree.h"

#define MAX_DEV_NAME_LENGTH 8		// Maximum zurich device name length -> dev8062 == 8
#define MAX_DEVICE_CONNECTIONS 16	// Maximum number of simultaneous zurich device connections in the main UI (internally we create more)
#define MAX_MEASUREMENTS 64			// Maximum number of measurement definitions the program can store(/run?) at once
#define MAX_NODE_PATH 128
#define MAX_MEAS_STEPS 10			// Maximum number of measurement steps allowed in a single measurement
#define MAX_MEAS_VARS 10			// Maximum number of variables in a measurement step

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
typedef struct IndependentVariable IndependentVariable;
typedef struct DependentVariable DependentVariable;


 // Stores everything there is to know about the entire program
struct ZMeasure {
	PrimaryPanels* panels;								// Panels for all of the primary functions of the UI
	ZurichConn* connections[MAX_DEVICE_CONNECTIONS];	// Array of connections to zurich devices
	size_t connCount;									// Number of active connections to zurich devices
	ActiveConn* activeConn;								// Connection displayed in the main UI window
	Measurement* measurements[MAX_MEASUREMENTS];		// Array of each defined measurement
	size_t measurementCount;							// Number of currently defined measurements
};

struct ActiveConn {
	int timer;					// Async timer reference
	ZurichConn* conn;			// Zurich connection used in the Async timer thread
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


enum MeasAction {MEAS_ACTION_NONE, MEAS_ACTION_STOP, MEAS_ACTION_PAUSE, MEAS_ACTION_RESUME};

// A definition of a Measurement
// A Measurement is made up of one ore more Measurement steps
// Each step is made up of zero or more Measurement variables
// (zero for repeated Measurements for averaging)
struct Measurement {
	char* name;							// Name of the measurement
	uint32_t nSteps;					// Number of Measurement steps
	MeasStep* steps[MAX_MEAS_STEPS];	// Array of each Measurement step
	ZMeasure* zmeasure;					// Parent program
	int panel;							// Settings panel for this measurement
	ZurichConnDef* connDef;				// Zurich connection definitions
	CmtThreadLockHandle threadLock;		// Thread lock
	CmtThreadFunctionID threadID;		// Measurement thread ID
	enum MeasAction requestAction;		// Requested action such as stop, pause, resume, etc.
	TreeNode* measTree;					// User specified measurement nodes
};


// Type of sweep to generate
// Single -> Simple sweep in one direction
// Dual -> Sweep forward and then repeat the sweep in reverse
// Piecewise -> Sweep along a piecewise defined set of points (Not yet implemented)
enum SweepType{SWEEP_TYPE_SINGLE, SWEEP_TYPE_DUAL, SWEEP_TYPE_PIECEWISE};  

// Type of variable
// Independent -> Variable value is its own thing
// Dependent -> Variable value is relative to another variable
enum VarType{VAR_TYPE_INDEPENDENT, VAR_TYPE_DEPENDENT};

// Information about a Measurement step.
struct MeasStep {
	char* name;						// Name of the measurement step
	uint32_t nPoints;				// Number of points in this Measurement step
	uint32_t nVars;					// Number of measurement variables at this step
	MeasVar* vars[MAX_MEAS_VARS];	// Array of variables to modify at this Measurement step
	float delay;					// Time delay, in seconds, between each measurement point at this step
	Measurement* parent;			// Parent measurement
	enum SweepType sweepType;		// Which type of sweep
	int panel;						// Panel editing this step
};

// This variable is a regular linear sweep
struct IndependentVariable {
	double start;		// Starting value
	double step;		// Increment value
};

// This variable is relative to another variable
struct DependentVariable {
	MeasVar* ref;		// Variable that this variable is relative to
	double coeff;		// Multiplicative coefficient. This variable's value is coeff * value of ref
};
 
struct MeasVar {
	char* name;					// Name of the variable
	ZurichConn* conn;			// Connection to use for this variable
	double coeff;				// Measurement coefficient. Multiplied by raw values to get data in desired units
	char* path;					// Path to the node to set at each step
	union {
		IndependentVariable independentVariable;
		DependentVariable dependentVariable;
	};
	double* values;				// Array of precomputed values for the sweep
	MeasStep* parent;			// Parent MeasStep
	enum VarType varType;			// Type of variable
};



/***** Function Prototypes *****/

// Memory management
ZurichConnDef* newZurichConnDef(char* address, uint16_t port, char* device);
ZurichConnDef* copyZurichConnDef(ZurichConnDef* connDefOri);
void deleteZurichConnDef(ZurichConnDef* connDef);
ZurichConn* newZurichConn(ZurichConnDef* connDef);
void deleteZurichConn(ZurichConn* zurich);
Measurement* newMeasurement(ZMeasure* zmeasure);
void deleteMeasurement(Measurement* measurement);
MeasStep* newMeasStep(Measurement* measurement);
void deleteMeasStep(MeasStep* measStep);
MeasVar* newMeasVar(MeasStep* measStep);
void deleteMeasVar(MeasVar* measVar);

// Utility
size_t getZurichConnIndex(ZMeasure* zmeasure, ZurichConn* zurich);
int addZurichConnToZMeasure(ZMeasure* zmeasure, ZurichConn* zurich);
int removeZurichConnFromZMeasure(ZMeasure* zmeasure, ZurichConn* zurich);
long int getMeasStepIndex(Measurement* measurement, MeasStep* measStep);
long int getMeasurementIndex(ZMeasure* zmeasure, Measurement* measurement);
int addMeasurementToZMeasure(ZMeasure* zmeasure, Measurement* measurement);
int removeMeasurementFromZMeasure(ZMeasure* zmeasure, Measurement* measurement);

#endif  /* ndef __meastypes_H__ */
