#ifndef __MeasurementLegacyNODES_H__
#define __MeasurementLegacyNODES_H__

struct MeasurementLegacyNodes {
	char** paths;
	int count;
};

void addMeasurementLegacyNode(struct MeasurementLegacyNodes* nodes, char* path);
void removeMeasurementLegacyNode(struct MeasurementLegacyNodes* nodes, int index);
int getMeasurementLegacyNodeIndex(struct MeasurementLegacyNodes* nodes, char* path);

void freeMeasurementLegacyNodes(struct MeasurementLegacyNodes* nodes);

#endif  /* ndef __MeasurementLegacyNodes_H__ */
