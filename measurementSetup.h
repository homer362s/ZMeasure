#ifndef __measurementSetup_H__
#define __measurementSetup_H__


void connectToZurich(struct Measurement* measurement);
void disconnectFromZurich(struct Measurement* measurement, struct ZurichData* zurich);

struct ZurichData* newZurichConnection(struct Measurement* measurement);
void deleteZurichConnection(struct Measurement* measurement, int index);


#endif  /* ndef __measurementSetuph_H__ */
