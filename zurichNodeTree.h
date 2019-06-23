#ifndef __ZURICHNODETREE_H__
#define __ZURICHNODETREE_H__


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


#endif
