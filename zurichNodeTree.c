// Handles storing a zurich device's node tree

#include <ansi_c.h>  
#include <userint.h> 

#include "fixedziAPI.h"
#include "meastypes.h"

#include "measurementSetup.h"
#include "zurichNodeTree.h"

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
void getNodeTreeFromDevice(struct ZurichData* zurich, struct ZurichNode* tree, uint32_t flags)
{
	char* nodestr = malloc(ZURICH_NODESTR_MAXLEN * sizeof(char));
	
	getZurichTreePath(tree, zurich->path);
	zurich->retVal = ziAPIListNodes(zurich->conn, zurich->path, nodestr, ZURICH_NODESTR_MAXLEN, flags);
	
	
	// Count returned nodes and break into substrings
	int nodeCount = 0;
	char c;
	int charCount;
	for(charCount = 0;charCount < ZURICH_NODESTR_MAXLEN;charCount++) {
		c = nodestr[charCount];
		
		if(!c) {
			if (nodeCount)
				nodeCount = nodeCount + 1;
			break;
		}
		
		if(c == '\n') {
			nodeCount = nodeCount + 1;
			nodestr[charCount] = 0;
		}
	}
	
	// Copy names into tree
	if(nodeCount) {
		tree->nChildren = nodeCount;
		
		// Allocate children array, individual children, and initialize
		tree->children = malloc(nodeCount * sizeof(struct ZurichNode*));
		for(int i = 0;i < nodeCount;i++) {
			tree->children[i] = malloc(sizeof(struct ZurichNode));
			tree->children[i]->parent = tree;
			tree->children[i]->nChildren = -1;
		}		
		
		// Set names for each child
		size_t namelen;
		size_t strStart = 0;
		for(int i = 0;i < nodeCount;i++) {
			// Copy the name into a new string
			namelen = strlen(nodestr + strStart);
			tree->children[i]->name = malloc((namelen + 1) * sizeof(char));
			strcpy(tree->children[i]->name, nodestr + strStart);
			
			
			// Find the start of the next string
			strStart = strStart + namelen + 1;
		}
	
		// Recursively fill in the remaining sub children
		for(int i = 0;i < tree->nChildren;i++) {
			getNodeTreeFromDevice(zurich, tree->children[i], flags);
		}
	} else {
		tree->nChildren = 0;
		tree->children = 0;
	}
	
	free(nodestr);
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

// Free all the children of a ZurichNode tree
void freeZurichNode(struct ZurichNode* tree)
{
	for(int i = 0;i < tree->nChildren;i++) {
		freeZurichNode(tree->children[i]);
	}
	free(tree->children);
	free(tree->name);
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

void populateTree(struct MeasurementLegacy* MeasurementLegacy, int treeControl, uint32_t flags)
{
	int zindex = MeasurementLegacy->connectionCount - 1;
	struct ZurichData* zurich = MeasurementLegacy->zurich[zindex];
	
	
	struct ZurichNode* tree = malloc(sizeof(struct ZurichNode));
	zurich->tree = tree;
	
	tree->name = malloc(ZURICH_DEV_MAXLEN * sizeof(char));
	strcpy(tree->name, zurich->device);
	tree->parent = 0;		// Tree root doesn't have a parent
	getNodeTreeFromDevice(zurich, tree, flags);
	
	
	// Note: CVI trees can be sorted with SortTreeItems().
	// But I think I want to have the node tree itself sorted as well
	sortNodeTree(tree);
	
	int rootIndex = InsertTreeItem(MeasurementLegacy->panels->znodes, treeControl, VAL_SIBLING, 0, VAL_LAST, zurich->device, NULL, 0, 0);
	// Store the tree pointer as an int because void* isn't a valid type for a cvi tree   
	SetTreeItemAttribute(MeasurementLegacy->panels->znodes, treeControl, rootIndex, ATTR_CTRL_VAL, (int)tree);		
	for(int i = 0;i < tree->nChildren;i++) {
		populateTreeNode(MeasurementLegacy->panels->znodes, treeControl, tree->children[i], rootIndex);
	}
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

