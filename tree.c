#include "tree.h"

#include <ansi_c.h> 


// Private function prototypes
static int getTreeNodeIndex(TreeNode* tree);
static void deleteTreeNode(TreeNode* tree, void delfcn(void* data));


// Allocate memory for a new tree
// allocSize -> Length of TreeNode.children to allocate
TreeNode* newTree(size_t allocSize)
{
	TreeNode* tree = malloc(sizeof(*tree));
	if (tree) {
		tree->allocSize = allocSize;
		tree->children = malloc(tree->allocSize * sizeof(*tree->children));
		for (size_t i = 0;i < tree->allocSize;i++) {
			tree->children[i] = 0;
		}
		tree->nChildren = 0;
		tree->data = 0;
		tree->parent = 0;
	}
	
	return tree;
}

// Delete a tree and free all the memory. Must be called on the parent
// it is an error if it is not and nothing is freed
// delfcn -> Function that takes TreeNode.data and frees it
// Returns 0 if successfully freed, 1 if there was an error
int deleteTree(TreeNode* tree, void delfcn(void* data))
{
	// If this tree isn't the parent, it is an error
	if (tree->parent) {
		return 1;
	}
	
	deleteTreeNode(tree, delfcn);
	
	return 0;
}

// Create a new child node
// tree -> Node for which to create a child
// allocSize -> Number of children to allocate space for
TreeNode* addNodeToTree(TreeNode* tree, size_t allocSize)
{
	TreeNode* childTree = newTree(allocSize);
	graftTree(tree, childTree);
	
	return childTree;
}

// Returns the first generation child node of a node containing the specified data 
// compfcn should return 0 if the two are equal
TreeNode* getTreeNodeFromData(TreeNode* tree, void* data, int (*compfcn)(void* data1, void* data2))
{
	if (!tree) {
		return 0;
	}
	for (size_t i = 0;i < tree->nChildren;i++) {
		if (compfcn(tree->children[i]->data, data) == 0) {
			return tree->children[i];
		}
	}
	
	return 0;
}

// Combine two trees into one
// baseTree -> Node to become the parent
// childTree -> Node which will become a child of the specified parent
void graftTree(TreeNode* baseTree, TreeNode* childTree)
{
	// Make sure the child isn't already part of another tree
	// detach it if it is
	detachNodeFromTree(childTree);
	
	// Make sure there is enough space, if not allocate more
	if (baseTree->nChildren >= baseTree->allocSize) {
		size_t newSize;
		if (baseTree->allocSize <= 5) {
			newSize = 10;
		} else if (baseTree->allocSize <= 20) {
			newSize = baseTree->allocSize * 2;
		} else {
			newSize = (size_t)(baseTree->allocSize * 1.5);
		}
		baseTree->children = realloc(baseTree->children, newSize * sizeof(TreeNode));
		baseTree->allocSize = newSize;
	}
	
	// Add child tree to parent's child list
	baseTree->children[baseTree->nChildren] = childTree;
	baseTree->nChildren += 1;
	childTree->parent = baseTree;
}

// Separate a node from a tree
// childTree -> Node to free from its parent
// Returns 0 on success, 1 on failure
int detachNodeFromTree(TreeNode* childTree)
{
	TreeNode* parent = childTree->parent;
	
	// If the node already is the root, we are done, return
	if (!parent) {
		return 0;
	}
	
	
	int index = getTreeNodeIndex(childTree);
	if (index < 0) {
		return 1;
	}
	
	// Shift siblings forward to fill gap
	for (size_t i = index+1;i < parent->nChildren;i++) {
		parent->children[i-1] = parent->children[i];
	}
	parent->nChildren -= 1;
	
	childTree->parent = 0;
	
	return 0;
}

// Delete a node in a tree
// childTree -> node to delete
// delfcn -> function to free data
void deleteNodeFromTree(TreeNode* childTree, void delfcn(void*))
{
	detachNodeFromTree(childTree);
	deleteTree(childTree, delfcn);
}


// Shallow copy of a tree
TreeNode* copyTree(TreeNode* sourceTree)
{
	// If source is the null pointer, do nothing
	if (!sourceTree) {
		return 0;
	}
	
	// Make copy
	TreeNode* copy = newTree(sourceTree->nChildren);
	copy->data = sourceTree->data;
	for(size_t i = 0;i < sourceTree->nChildren;i++) {
		copyTreeIntoTree(copy, sourceTree->children[i]);
	}
	
	return copy;
}

TreeNode* copyTreeIntoTree(TreeNode* parentTree, TreeNode* sourceTree)
{
	if (!sourceTree) {
		return 0;
	}
	
	TreeNode* copy = copyTree(sourceTree);
	
	graftTree(parentTree, copy);
	
	return copy;
}


TreeNode* copyTreeFromBase(TreeNode* sourceTree)
{
	// Copy specified tree
	TreeNode* copy = copyTree(sourceTree);
	TreeNode* parent;
	
	// Copy all the direct ancestors
	while((parent = copy->parent)) {
		TreeNode* newParent = newTree(1);
		newParent->data = parent->data;
		graftTree(newParent, copy);
		copy = newParent;
	}
	
	return copy;
}


// Iterate over the entire tree
// fcn(TreeNode* node, size_t depth) is called for each node, starting with the root
// node, then the first child, the the first grand child
void depthFirstIterTree(TreeNode* tree, size_t startingDepth, void* data, void (*fcn)(TreeNode* node, size_t depth, void* data))
{
	(*fcn)(tree, startingDepth, data);
	for(size_t i = 0;i < tree->nChildren;i++) {
		depthFirstIterTree(tree->children[i], startingDepth+1, data, fcn);
	}
} 

// Passed to depthFirstIterTree to print string data from a Tree
void printStrIterFcn(TreeNode* node, size_t depth, void* data)
{
	for(size_t i = 0;i < depth;i++) {
		printf(" ");
	}
	printf("%s\n", (char*)node->data);
}

// Passed to depthFirstIterTree to sort sting data in the tree
void sortStrIterFcn(TreeNode* node, size_t depth, void* data)
{
	TreeNode* tmp;
	
	// Sort this node
	for (size_t i = 1;i < node->nChildren;i++) {
		// Compare nodes i-1 and i
		if (strcmp(node->children[i-1]->data, node->children[i]->data) > 0) {
			tmp = node->children[i-1];
			node->children[i-1] = node->children[i];
			node->children[i] = tmp;
			i = 0;
		}
	}
}


/***** Helper functions *****/
// Pass to deleteNodeFromTree to not free the data within the nodes
void dontFree(void* ptr)
{
}


/***** Private functions *****/
static int getTreeNodeIndex(TreeNode* tree)
{
	TreeNode* parent = tree->parent;
	
	// If there is no parent then we can't find the index
	if (!parent) {
		return -1;
	}
	
	// Iterate over parent's children looking for ourself
	for (size_t i = 0;i < parent->nChildren;i++) {
		if (parent->children[i] == tree) {
			return (int)i;
		}
	}
	
	// Couldn't find
	return -1;
}


// Delete a tree node and free all its memory
// This should usually not be called since it doesn't properly
// clean up the parent's reference list
static void deleteTreeNode(TreeNode* tree, void delfcn(void* data))
{
	// Free children
	for(size_t i = 0;i < tree->nChildren;i++) {
		deleteTreeNode(tree->children[i], delfcn);
	}
	free(tree->children);
	tree->children = 0;
	
	// Free this node
	delfcn(tree->data);
	tree->data = 0;
	
	free(tree);
	tree = 0;
}


