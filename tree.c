#include "tree.h"

#include <ansi_c.h> 


// Private function prototypes
static int getTreeNodeIndex(TreeNode* tree);


// Allocate memory for a new tree
// allocSize -> Length of TreeNode.children to allocate
TreeNode* newTree(size_t allocSize)
{
	TreeNode* tree = malloc(sizeof(TreeNode));
	if (tree) {
		tree->allocSize = allocSize;
		tree->children = malloc(tree->allocSize * sizeof(TreeNode*));
		for (int i = 0;i < tree->allocSize;i++) {
			tree->children[i] = 0;
		}
		tree->nChildren = 0;
		tree->data = 0;
		tree->parent = 0;
	}
	
	return tree;
}

// Delete a tree and free all the memory
// delfcn -> Function that takes TreeNode.data and frees it
// Returns 0 if successfully freed, 1 if there was an error
int deleteTree(TreeNode* tree, void delfcn(void* data))
{
	// If this tree isn't the parent, it is an error
	if (tree->parent) {
		return 1;
	}
	
	// Free children
	for(int i = 0;i < tree->nChildren;i++) {
		tree->children[i];
	}
	free(tree->children);
	tree->children = 0;
	
	delfcn(tree->data);
	tree->data = 0;
	
	free(tree);
	tree = 0;
	
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
			newSize = baseTree->allocSize * 1.5;
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
	
	// If the node already is the parent, we are done, return
	if (!parent) {
		return 0;
	}
	
	
	int index = getTreeNodeIndex(childTree);
	if (index < 0) {
		return 1;
	}
	
	// Shift siblings forward to fill gap
	for (int i = index+1;i < parent->nChildren;i++) {
		parent->children[i-1] = parent->children[i];
	}
	parent->nChildren -= 1;
	
	return 0;
}

// Delete a node in a tree
// childTree -> node to delete
void deleteNodeFromTree(TreeNode* childTree, void delfcn(void*))
{
	detachNodeFromTree(childTree);
	deleteTree(childTree, delfcn);
}


// Iterate over the entire tree
// fcn(TreeNode* node, size_t depth) is called for each node, starting with the root
// node, then the first child, the the first grand child
void depthFirstIterTree(TreeNode* tree, size_t startingDepth, void (*fcn)(TreeNode* node, size_t depth))
{
	(*fcn)(tree, startingDepth);
	for(int i = 0;i < tree->nChildren;i++) {
		depthFirstIterTree(tree->children[i], startingDepth+1, fcn);
	}
} 

// Passed to depthFirstIterTree to print string data from a Tree
void printStrIterFcn(TreeNode* node, size_t depth)
{
	for(size_t i = 0;i < depth;i++) {
		printf(" ");
	}
	printf("%s\n", (char*)node->data);
}

// Passed to depthFirstIterTree to sort sting data in the tree
void sortStrIterFcn(TreeNode* node, size_t depth)
{
	TreeNode* tmp;
	
	// Sort this node
	for (int i = 1;i < node->nChildren;i++) {
		// Compare nodes i-1 and i
		if (strcmp(node->children[i-1]->data, node->children[i]->data) > 0) {
			tmp = node->children[i-1];
			node->children[i-1] = node->children[i];
			node->children[i] = tmp;
			i = 0;
		}
	}
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
	for (int i = 0;i < parent->nChildren;i++) {
		if (parent->children[i] == tree) {
			return i;
		}
	}
	
	// Couldn't find
	return -1;
}


// Pass to deleteNodeFromTree to not free the data within the nodes
void dontFree(void* ptr)
{
}
