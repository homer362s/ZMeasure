

#include <ansi_c.h>
#include <userint.h>

#include "fixedziAPI.h"

#include "measurementSetup.h"
#include "zurichNodeTree.h"
#include "ziUtility.h"

// Print the node tree to stdout
void printNodeTree(struct ZurichNode* tree, char* indent)
{
	char newIndent[10];
	sprintf(newIndent, "%s ", indent);
	for(int i = 0;i < tree->nChildren;i++) {
		printf("/%s%s\n", indent, tree->children[i]->name);
		printNodeTree(tree->children[i], newIndent);
	}
}

// Get the entire node tree from the connect zurich device
// ZurichData* zurich -> pointer to the zurich data/connection struct
// ZurichNode* tree -> all the children will be filled in for this node 
// int flags -> flags passed directly through to ziAPIListNodes
void getNodeTreeFromDevice(struct ZurichData* zurich, struct ZurichNode* tree, int flags)
{
	// TODO: nodestr should be heap memory rather than stack memory
	char nodestr[10000];
	
	getZurichTreePath(tree, zurich->path);
	zurich->retVal = ziAPIListNodes(zurich->conn, zurich->path, nodestr, sizeof(nodestr), flags);
	
	
	// Count returned nodes
	int nodeCount = 0;
	char c;
	int charCount;
	for(charCount = 0;charCount < 10000;charCount++) {
		c = nodestr[charCount];
		
		if(!c) {
			if (nodeCount)
				nodeCount = nodeCount + 1;
			break;
		}
		
		if(c == '\n') {
			nodeCount = nodeCount + 1;
		}
	}
	
	// Copy names into tree
	if(nodeCount) {
		tree->nChildren = nodeCount; 
		tree->nameptr = malloc(nodeCount + charCount * sizeof(char));
		tree->children = malloc(nodeCount * sizeof(struct ZurichNode*));
		tree->nodeptr = malloc(nodeCount * sizeof(struct ZurichNode));
		// Map children node pointers in the array
		for(int i = 0;i < nodeCount;i++) {
			tree->children[i] = tree->nodeptr + i;
		}
		strcpy(tree->nameptr, nodestr);
		
		// Set children's parent
		for(int i = 0;i < tree->nChildren;i++) {
			tree->children[i]->parent = tree;
		}
	} else {
		tree->nChildren = 0;
		tree->nameptr = 0;
		tree->children = 0;
		return;
	}
	
	// Set names for each child by pointing to the start of the substring in .ptr
	int childIndex = 0;
	tree->children[childIndex]->name = tree->nameptr;
	tree->children[childIndex]->nChildren = -1;
	childIndex = 1;
	int i = 0;
	while(childIndex < nodeCount) {
		c = tree->nameptr[i];
		
		if(c == 0) {
			break;
		}
		
		if(c == '\n') {
			tree->nameptr[i] = 0;
			tree->children[childIndex]->name = tree->nameptr + i + 1;
			tree->children[childIndex]->nChildren = -1;
			childIndex = childIndex + 1;
		}
		
		i = i + 1;
	}
	
	// Recursively fill in the remaining sub children
	for(i = 0;i < tree->nChildren;i++) {
		getNodeTreeFromDevice(zurich, tree->children[i], flags);
	}
}

void sortNodeTree(struct ZurichNode* tree)
{
	struct ZurichNode* tmp;
	
	// Sort this node
	for(int i = 1;i < tree->nChildren;i++) {
		// Compare nodes i-1 and i
		if(strcmp(tree->children[i-1]->name, tree->children[i]->name) > 0) {
			tmp = tree->children[i-1];
			tree->children[i-1] = tree->children[i];
			tree->children[i] = tmp;
			i = 0;
		}
	}
	
	// Sort child nodes
	for(int i = 0;i < tree->nChildren;i++) {
		sortNodeTree(tree->children[i]);
	}
}

// Free all the children of a ZurichNode tree and all children
void freeZurichNodeChildren(struct ZurichNode* tree)
{
	for(int i = 0;i < tree->nChildren;i++) {
		freeZurichNodeChildren(tree->children[i]);
	}
	free(tree->nameptr);
	free(tree->children);
}
																																		 
// Free an entire ZurichNode tree. Must be called on the root of a tree
void freeZurichNodeTree(struct ZurichNode* tree)
{
	freeZurichNodeChildren(tree);
	free(tree);
}

void populateTreeNode(int panel, int treeControl, struct ZurichNode* tree, int parentIndex)
{
	int index = InsertTreeItem(panel, treeControl, VAL_CHILD, parentIndex, VAL_LAST, tree->name, NULL, 0, 0);
	SetTreeItemAttribute(panel, treeControl, index, ATTR_CTRL_VAL, (int)tree); 
	SetTreeItemAttribute(panel, treeControl, index, ATTR_COLLAPSED, 1);
	for(int i = 0;i < tree->nChildren;i++) {
		populateTreeNode(panel, treeControl, tree->children[i], index);
	}
}

void populateTree(struct Measurement* measurement, int treeControl, int flags)
{
	int zindex = measurement->connectionCount - 1;
	struct ZurichData* zurich = measurement->zurich[zindex];
	
	
	struct ZurichNode* tree = malloc(sizeof(struct ZurichNode));
	zurich->tree = tree;
	
	tree->name = zurich->device;
	tree->parent = 0;		// Tree root doesn't have a parent
	getNodeTreeFromDevice(zurich, tree, flags);
	
	
	// Note: CVI trees can be sorted with SortTreeItems().
	// But I think I want to have the node tree itself sorted as well
	sortNodeTree(tree);
	
	int rootIndex = InsertTreeItem(measurement->panels->znodes, treeControl, VAL_SIBLING, 0, VAL_LAST, zurich->device, NULL, 0, 0);
	// Store the tree pointer as an int because void* isn't a valid type for a cvi tree   
	SetTreeItemAttribute(measurement->panels->znodes, treeControl, rootIndex, ATTR_CTRL_VAL, (int)tree);		
	for(int i = 0;i < tree->nChildren;i++) {
		populateTreeNode(measurement->panels->znodes, treeControl, tree->children[i], rootIndex);
	}
	
	
	freeZurichNodeTree(tree);
}
	   

void getZurichTreePath(struct ZurichNode* leaf, char* fullPath)
{
	struct ZurichNode* parent;
	struct ZurichNode* node = leaf;
	
	// Initialize path to be an empty string
	char partialPath[128];
	partialPath[0] = 0;
	
	// Build up path from right to left
	while((parent = (node->parent))) {
		sprintf(fullPath, "%s/%s", node->name, partialPath);
		node = parent;
		strcpy(partialPath, fullPath);
	}
	
	// Add the root node to finish the path
	sprintf(fullPath, "/%s/%s", node->name, partialPath);
}



void addZurichNode(struct ZurichNode* rootNode, struct ZurichNode* parent)
{
}


void removeZurichNode(struct ZurichNode* targetNode)
{
}

