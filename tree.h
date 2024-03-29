#ifndef __TREE_H__
#define __TREE_H__

#include <stdint.h>

typedef struct TreeNode TreeNode;


struct TreeNode {
	struct TreeNode* parent;
	void* data;							// Data stored in this node
	size_t nChildren;					// Number of child nodes
	struct TreeNode** children;			// array of child nodes
	size_t allocSize;					// malloc length of TreeNode.children;
};
		
// Allocate memory for a new tree
// allocSize -> Length of TreeNode.children to allocate
TreeNode* newTree(size_t allocSize);

// Delete a tree and free all the memory
// delfcn -> Function that takes TreeNode.data and frees it
int deleteTree(TreeNode* tree, void delfcn(void* data));

// Create a new child node
// tree -> Node for which to create a child
// allocSize -> Number of children to allocate space for
TreeNode* addNodeToTree(TreeNode* tree, size_t allocSize);

// Combine two trees into one
// baseTree -> Node to become the parent
// childTree -> Node which will become a child of the specified parent
void graftTree(TreeNode* baseTree, TreeNode* childTree);

// Separate a node from a tree
// childTree -> Node to free from its parent
int detachNodeFromTree(TreeNode* childTree);

// Delete a node in a tree
// childTree -> node to delete
void deleteNodeFromTree(TreeNode* childTree, void delfcn(void*));

// Shallow copy a tree
// Returns a pointer to the new tree
TreeNode* copyTree(TreeNode* sourceTree);

// Same as copyTree but grafts the copy into a specified node
TreeNode* copyTreeIntoTree(TreeNode* parentTree, TreeNode* sourceTree);

// Shallow copy a tree, including all nodes that lead to it
// Does not copy siblings, or cousins, etc.
TreeNode* copyTreeFromBase(TreeNode* sourceTree);

// Returns the first generation child node of a node containing the specified data 
// compfcn should return 0 if the two are equal
TreeNode* getTreeNodeFromData(TreeNode* tree, void* data, int (*compfcn)(void* data1, void* data2));

// Iterate over the entire tree
// fcn(void* data) is called for each node, starting with the root
// node, then the first child, the the first grand child
void depthFirstIterTree(TreeNode* tree, size_t startingDepth, void* data, void (*fcn)(TreeNode* node, size_t depth, void* data));
void printStrIterFcn(TreeNode* node, size_t depth, void* data);
void sortStrIterFcn(TreeNode* node, size_t depth, void* data);

// Dummy function passed to deleteNodeFromTree if you don't want
// The node data freed
void dontFree(void* ptr);

#endif  /* ndef __tree_H__ */
