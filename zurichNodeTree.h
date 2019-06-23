#ifndef __ZURICHNODETREE_H__
#define __ZURICHNODETREE_H__

#include "measurementSetup.h"

struct ZurichNode {
	struct ZurichNode* parent;  
	char* name;
	int nChildren;
	struct ZurichNode** children;
	char* nameptr;						// Pointer to memory allocated for children names
	struct ZurichNode* nodeptr;			// Pointer to memory allocated for children ZurichNodes
};


struct ZurichNode* getZurichTreeRoot(struct ZurichNode* tree);
void getZurichTreePath(struct ZurichNode* tree, char* path);
void populateTree(struct Measurement* measurement, int treeControl, int flags);
void populateTreeNode(int panel, int treeControl, struct ZurichNode* tree, int parentIndex);

void freeZurichNodeTree(struct ZurichNode* tree);
void freeZurichNodeChildren(struct ZurichNode* tree);

void sortNodeTree(struct ZurichNode* tree);

void getNodeTreeFromDevice(struct ZurichData* zurich, struct ZurichNode* tree, int flags);

void printNodeTree(struct ZurichNode* tree, char* indent);

void addZurichNode(struct ZurichNode* rootNode, struct ZurichNode* parent);
void removeZurichNode(struct ZurichNode* targetNode); 


#endif
