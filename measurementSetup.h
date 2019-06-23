#ifndef __measurementSetup_H__
#define __measurementSetup_H__

#define MAX_DEVICE_LENGTH 8
#define MAX_DEVICE_CONNECTIONS 16

struct PanelHandles {
	int main;
	int about;
	int zconn;
	int znodes;
	int sweep;
	int output;			 
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
	struct ZurichData* zurich[MAX_DEVICE_CONNECTIONS];
};

void connectToZurich(struct Measurement* measurement);
void disconnectFromZurich(struct Measurement* measurement, struct ZurichData* zurich);

struct ZurichData* newZurichConnection(struct Measurement* measurement);
void deleteZurichConnection(struct Measurement* measurement, int index);


#endif  /* ndef __measurementSetuph_H__ */
