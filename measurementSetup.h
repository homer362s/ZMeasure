#ifndef __MeasurementLegacySetup_H__
#define __MeasurementLegacySetup_H__


void connectToZurich(struct MeasurementLegacy* MeasurementLegacy);
void disconnectFromZurich(struct MeasurementLegacy* MeasurementLegacy, struct ZurichData* zurich);

struct ZurichData* newZurichConnection(struct MeasurementLegacy* MeasurementLegacy);
void deleteZurichConnection(struct MeasurementLegacy* MeasurementLegacy, int index);

ZMeasure* allocateSystemVars();
void deleteSystemVars(ZMeasure* zmeasure);
ZurichConnDef* newZurichConnDef(char* address, uint16_t port, char* device);
void deleteZurichConnDef(ZurichConnDef* connDef);
void getConnName(ZurichConnDef* connDef, char* name);
ZurichConn* newZurichConn(ZurichConnDef* connDef); 
void deleteZurichConn(ZurichConn* zurich);

ActiveConn* newActiveConn(ZurichConnDef* connDef);
void deleteActiveConn(ZurichConn* zurich);

ZurichConnDef* copyZurichConnDef(ZurichConnDef* connDefOri);


#endif  /* ndef __MeasurementLegacySetuph_H__ */
