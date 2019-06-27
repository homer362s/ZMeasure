#ifndef __MEASUREMENTNODES_H__
#define __MEASUREMENTNODES_H__

struct MeasurementNodes {
	char** paths;
	int count;
};

void addMeasurementNode(struct MeasurementNodes* nodes, char* path);
void removeMeasurementNode(struct MeasurementNodes* nodes, int index);
int getMeasurementNodeIndex(struct MeasurementNodes* nodes, char* path);

void freeMeasurementNodes(struct MeasurementNodes* nodes);

#endif  /* ndef __measurementNodes_H__ */
