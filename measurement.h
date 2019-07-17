#ifndef __MEASUREMENT_H__
#define __MEASUREMENT_H__

// Necessary includes for this module
#include "meastypes.h"

void initMeasNodes(void);
void deleteMeasNodes(void);
void raiseMeasurementPanel(Measurement* measurement);
void createMeasurementInUI(ZMeasure* zmeasure);
void deleteMeasurementFromUI(Measurement* measurement);

#endif  /* ndef __MeasurementLegacy_H__ */
