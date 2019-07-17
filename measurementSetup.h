#ifndef __MeasurementLegacySetup_H__
#define __MeasurementLegacySetup_H__

#include "meastypes.h"


ZMeasure* allocateSystemVars(void);
void deleteSystemVars(ZMeasure* zmeasure);
ZurichConnDef* newZurichConnDef(char* address, uint16_t port, char* device);
void deleteZurichConnDef(ZurichConnDef* connDef);
void getConnName(ZurichConnDef* connDef, char* name);
ZurichConn* newZurichConn(ZurichConnDef* connDef); 
void deleteZurichConn(ZurichConn* zurich);

ActiveConn* newActiveConn(ZurichConnDef* connDef);
void deleteActiveConn(ZurichConn* zurich);

ZurichConnDef* copyZurichConnDef(ZurichConnDef* connDefOri);

void autophase(int panel, int control);
int readNumberScientific(char* instr, double* value);


void deleteUITimerThread(ZMeasure* zmeasure);
void newUITimerThread(ZMeasure* zmeasure, ZurichConnDef* oldConnDef);


#endif  /* ndef __MeasurementLegacySetuph_H__ */
