#ifndef __ZURICHNODETREE_H__
#define __ZURICHNODETREE_H__

#include "measurementSetup.h"

#define ZURICH_NODESTR_MAXLEN 5000
#define ZURICH_DEV_MAXLEN 8

struct ZurichNode {
	struct ZurichNode* parent;  
	char* name;
	int nChildren;
	struct ZurichNode** children;
};


struct ZurichNode* getZurichTreeRoot(struct ZurichNode* tree);
void getZurichTreePath(struct ZurichNode* tree, char* path);
void populateTree(struct MeasurementLegacy* MeasurementLegacy, int treeControl, uint32_t flags);
void populateTreeNode(int panel, int treeControl, struct ZurichNode* tree, int parentIndex);

void freeZurichNode(struct ZurichNode* tree);

void sortNodeTree(struct ZurichNode* tree);

void getNodeTreeFromDevice(struct ZurichData* zurich, struct ZurichNode* tree, uint32_t flags);

void printNodeTree(struct ZurichNode* tree, char* indent);

void addZurichNode(struct ZurichNode* rootNode, struct ZurichNode* parent);
void removeZurichNode(struct ZurichNode* targetNode); 


#endif
