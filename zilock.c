#include <userint.h>
#include <ansi_c.h>
#include "ziAPI.h" 
#include "zilock.h"

ZIResult_enum retVal;
ZIConnection conn;
char* errBuffer;
//const char serverAddress[] = "localhost";
const char serverAddress[] = "newsix.ee.nd.edu";

int testZISetParam() {
	// Connect to ZI LI
	retVal = ziAPIInit(&conn);
	if (retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(retVal, &errBuffer, NULL);
		fprintf(stderr, "Can't init Connection: %s\n", errBuffer);
		return 1;
	}
	
	// Connect to the Data Server: Use port 8005 for the HF2 Data Server, use
	// 8004 for the UHF and MF Data Servers. HF2 only support ZI_API_VERSION_1,
	// see the LabOne Programming Manual for an explanation of API Levels.
	retVal = ziAPIConnectEx(conn, serverAddress, 8004, ZI_API_VERSION_6, NULL);
	if (retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(retVal, &errBuffer, NULL);
		fprintf(stderr, "Error, can't connect to the Data Server: `%s`.\n", errBuffer);
	} else {
		// Set all demodulator rates of device dev2086 to 150 Hz
		retVal = ziAPISetValueD(conn, "/dev2086/oscs/7/freq", 150);
		if (retVal != ZI_INFO_SUCCESS) {
			ziAPIGetError(retVal, &errBuffer, NULL);
			fprintf(stderr, "Can't set parameter: %s\n", errBuffer);
		}
		// Disconnect from the Data Server. Since ZIAPIDisconnect always returns
		// ZI_INFO_SUCCESS no error handling is required.
		ziAPIDisconnect(conn);
	}
	
	// Destroy the ZI connection
	ziAPIDestroy(conn);
	
	return 0;
}







// Every second
int testZIStream() {
	
	// Connect to ZI LI
	retVal = ziAPIInit(&conn);
	if (retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(retVal, &errBuffer, NULL);
		fprintf(stderr, "Can't init Connection: %s\n", errBuffer);
		return 1;
	}
	
	// Connect to the Data Server: Use port 8005 for the HF2 Data Server, use
	// 8004 for the UHF and MF Data Servers. HF2 only support ZI_API_VERSION_1,
	// see the LabOne Programming Manual for an explanation of API Levels.
	retVal = ziAPIConnectEx(conn, serverAddress, 8004, ZI_API_VERSION_6, NULL);
	if (retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(retVal, &errBuffer, NULL);
		fprintf(stderr, "Error, can't connect to the Data Server: `%s`.\n", errBuffer);
		
		ziAPIDestroy(conn);
		return 1;
	} 
	

	// Set demodulator rates of device dev2086 to 150 Hz
	retVal = ziAPISetValueD(conn, "/dev2086/oscs/7/freq", 150);
	if (retVal != ZI_INFO_SUCCESS) {
		ziAPIGetError(retVal, &errBuffer, NULL);
		fprintf(stderr, "Can't set parameter: %s\n", errBuffer);
	}
	
	ZIResult_enum RetVal;
	char* ErrBuffer;

	ZIEvent* Event;
	unsigned int Cnt = 0;

	/*
	Allocate ZIEvent in heap memory instead of getting it from stack will
	secure against stack overflows especially in windows.
	*/
	if ((Event = ziAPIAllocateEventEx()) == NULL) {
		fprintf(stderr, "Can't allocate memory\n");
		return 1;
	}

	// Subscribe to a node, e.g., a demodulator sample.
	if ((RetVal = ziAPISubscribe(conn, "/dev2086/demods/7/sample")) != ZI_INFO_SUCCESS) {
		ziAPIGetError(RetVal, &ErrBuffer, NULL);
		fprintf(stderr, "Error, can't subscribe: %s\n", ErrBuffer);

		ziAPIDeallocateEventEx(Event);

		return 1;
	}

	// loop 1000 times
	while (Cnt < 1000) {
		// get all demod rates from all devices every 10th cycle
		if (++Cnt % 10 == 0) {
			if ((RetVal = ziAPIGetValueAsPollData(conn, "/dev2086/demods/7/sample")) != ZI_INFO_SUCCESS) {
				ziAPIGetError(RetVal, &ErrBuffer, NULL);
				fprintf(stderr, "Error, can't get value as poll data: %s.\n", ErrBuffer);
				break;
			}
		}

		// Poll data until no more data is available.
		while (1) {
			ProcessSystemEvents();
			ProcessDrawEvents();
			if ((RetVal = ziAPIPollDataEx(
				conn, Event, 0)) != ZI_INFO_SUCCESS) {
				ziAPIGetError(RetVal, &ErrBuffer, NULL);
				fprintf(stderr, "Error, can't poll data: %s.\n", ErrBuffer);

				break;
			} else {
				// The field Count of the Event struct is zero when no data has been
				// polled
				if (Event->valueType != ZI_VALUE_TYPE_NONE && Event->count > 0) {
					printf("Processing data\n");
				} else {
					printf("no more data\n");
					break;
				}
			}
		}
	}

	if (ziAPIUnSubscribe(conn, "*") != ZI_INFO_SUCCESS) {
		ziAPIGetError(RetVal, &ErrBuffer, NULL);
		fprintf(stderr, "Error, can't unsubscribe: %s.\n", ErrBuffer);
	}

	ziAPIDeallocateEventEx(Event);
	
	// Clean up connections
	ziAPIDisconnect(conn);
	ziAPIDestroy(conn);
	
	return 0;
}
