// A data structure to hold the specified measurement nodes

#include <ansi_c.h>

#include "measurementNodes.h"

void addMeasurementNode(struct MeasurementNodes* nodes, char* path)
{
	int len = strlen(path);
	nodes->count = nodes->count + 1;
	
	// Grow the array of paths
	nodes->paths = realloc(nodes->paths, nodes->count * sizeof(char*));
	nodes->paths[nodes->count - 1] = malloc((len + 1) * sizeof(char));
	strcpy(nodes->paths[nodes->count-1], path);
}

void removeMeasurementNode(struct MeasurementNodes* nodes, int index)
{
	// Free the specified path
	free(nodes->paths[index]);
	
	// Shift following paths forward
	for(int i = index+1;i < nodes->count;i++) {
		nodes->paths[i-1] = nodes->paths[i];
	}
	nodes->count = nodes->count - 1;
	
	// Shrink the array of paths
	nodes->paths = realloc(nodes->paths, nodes->count * sizeof(char*));
}

// Get the measurement node index for the specified path. Returns -1 if not found
int getMeasurementNodeIndex(struct MeasurementNodes* nodes, char* path)
{
	for(int i = 0;i < nodes->count;i++) {
		if(strcmp(nodes->paths[i], path) == 0) {
			return i;
		}
	}
	return -1;
}

// Free the entire MeasurementNode struct and all contained memory
void freeMeasurementNodes(struct MeasurementNodes* nodes)
{
	for(int i = 0;i < nodes->count;i++) {
		free(nodes->paths[i]);
	}
	free(nodes->paths);
	free(nodes);
}
