#ifndef __MEASUREMENT_H__
#define __MEASUREMENT_H__

// Necessary includes for this module
#include "meastypes.h"

// Defined constants
#define MEAS_STOPPED 0
#define MEAS_RUNNING 1
#define MEAS_PAUSED 2

// Datatypes
struct TreeSelectParams {
	void (*function)(void* callbackdata, TreeNode** nodeList);
	void* data;
	int leavesOnly;
};

// Function prototypes
void initMeasNodes(void);
void deleteMeasNodes(void);
void raiseMeasurementPanel(Measurement* measurement);
void createMeasurementInUI(ZMeasure* zmeasure);
void deleteMeasurementFromUI(Measurement* measurement);

#endif  /* ndef __Measurement_H__ */
