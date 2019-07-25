#include "measurement.h"
#include "ZMeasure.h"

// Contains functions relating to physically taking a measurement
#include "measurement.h"

#include <userint.h>   
#include <utility.h>  

#include "measurement_u.h"
#include "zmeasure.h"
#include "tree.h"
#include "str.h"

/***** Private function prototypes *****/
static void setUIState(int panel, int state);
static void startMeasurement(Measurement* measurement);
static void pauseMeasurement(Measurement* measurement);
static void stopMeasurement(Measurement* measurement);

static int getChildNodeIndex(TreeNode* tree, char* name);
static void addPathToTree(TreeNode* tree, char* path);
static void removePathFromTree(TreeNode* tree, char* path);
static void populateTree(int panel, int treeControl, struct TreeNode* tree);

static void outVarsSelect_CB(Measurement* measurement, TreeNode** nodeList);
static TreeNode** getTreeSelectedItems(int panel, int control);

int CVICALLBACK measurementThread (void* measurement);

/***** Functions to work with zurich nodes *****/
static const TreeNode* measNodes;
static const TreeNode* varNodes;

// Initialize the list of supported measurement nodes
void initMeasNodes()
{
	// Initialize measNodes
	char str[MAX_PATH_LEN];
	measNodes = newTree(5);
	
	// Aux inputs by themselves
	addPathToTree(measNodes, "/dev2086/auxins/0/sample");
	addPathToTree(measNodes, "/dev2086/auxins/1/sample");
	
	// Demods
	for (int i = 0;i < 8;i++) {
		sprintf(str, "/dev2086/demods/%d/sample/x", i);
		addPathToTree(measNodes, str);
		sprintf(str, "/dev2086/demods/%d/sample/y", i);
		addPathToTree(measNodes, str);
		sprintf(str, "/dev2086/demods/%d/sample/freq", i);
		addPathToTree(measNodes, str);
		sprintf(str, "/dev2086/demods/%d/sample/phase", i);
		addPathToTree(measNodes, str);
		sprintf(str, "/dev2086/demods/%d/sample/dioBits", i);
		addPathToTree(measNodes, str);
		sprintf(str, "/dev2086/demods/%d/sample/trigger", i);
		addPathToTree(measNodes, str);
		sprintf(str, "/dev2086/demods/%d/sample/auxIn0", i);
		addPathToTree(measNodes, str);
		sprintf(str, "/dev2086/demods/%d/sample/auxIn1", i);
		addPathToTree(measNodes, str);
	}
	
	
	// Initialize varnodes
	varNodes = newTree(5);
	for (int i = 0;i < 8;i++) {
		sprintf(str, "/dev2086/auxouts/%d/offset", i);
		addPathToTree(varNodes, str);
		sprintf(str, "/dev2086/oscs/%d/freq", i);
		addPathToTree(varNodes, str);
	}
}

// Frees the measNodes global struct
void deleteMeasNodes()
{
	deleteTree(measNodes, free);
	measNodes = 0;
	deleteTree(varNodes, free);
	varNodes = 0;
}


// Get index of the specified name as a direct child of the tree node
// Returns -1 if its not found
static int getChildNodeIndex(TreeNode* tree, char* name)
{
	for (size_t i = 0;i < tree->nChildren;i++) {
		if (strcmp(tree->children[i]->data, name) == 0) {
			return (int)i;
		}
	}
	
	return -1;
}

// Add a path to a tree
// Does not duplicate nodes that already exist
static void addPathToTree(TreeNode* tree, char* path)
{
	TreeNode* node = tree;
	int childIndex;
	int nNodes = 0;
	
	// Split path into individual parts and get the count
	StringList pathItems = splitString(path+1, '/');
	while (1) {
		if (!pathItems[nNodes]) {
			break;
		}
		nNodes++;
	}
	
	
	for (int i = 0;i < nNodes;i++) {
		childIndex = getChildNodeIndex(node, pathItems[i]);
		
		if (childIndex >= 0) {
			// Node already exists. Descend into it
			node = node->children[childIndex];
		} else {
			// Node does not exist, create it
			node = addNodeToTree(node, 5);
			size_t len = strlen(pathItems[i]);
			node->data = malloc((len+1) * sizeof(char));
			strcpy(node->data, pathItems[i]);
		}
	}
	
	deleteStringList(pathItems);
}

// Get the path up to and including the specified node
static void getPathFromTree(TreeNode* tree, char* path)
{
	char oldpath[MAX_PATH_LEN];
	strcpy(oldpath, tree->data);
	TreeNode* parent;
	while (tree = tree->parent) {
		if (tree->data && ((char*)(tree->data))[0]) {
			sprintf(path, "%s/%s", (char*)tree->data, oldpath);
			strcpy(oldpath, path);
		}
	}
	sprintf(path, "/%s", oldpath);
}


// Removes a path from a tree
// If the node to be removed has no siblings, its paren't is removed
// This continues recursively until the entire tree has been removed
// or until a node is found with siblings.
static void removePathFromTree(TreeNode* tree, char* path)
{
	char* token = strtok(path, "/");
	TreeNode* child = tree;
	
	do {
		child = getTreeNodeFromData(child, token, strcmp);
		if (!child) {
			return;
		}
	} while (token = strtok(0, "/"));
	
	TreeNode* parent = child->parent;
	while (parent = child->parent) {
		if (parent->nChildren > 1) {
			deleteNodeFromTree(child, free);
			break;
		}
		deleteNodeFromTree(child, free);
		child = parent;
	}
}

/***** Functions for modifying a measurement *****/

static void updateMeasurementDisplay(Measurement* measurement)
{
	int panel = measurement->panel;
	// Delete everything
	DeleteListItem(panel, MEASP_STEPTREE, 0, -1);
	
	int stepTreeItem = 0;
	// Loop over steps
	for (size_t i = 0;i < measurement->nSteps;i++) {
		// Add step
		MeasStep* thisStep = measurement->steps[i];
		stepTreeItem = InsertTreeItem(panel, MEASP_STEPTREE, VAL_SIBLING, stepTreeItem, VAL_LAST, thisStep->name, NULL, NULL, (unsigned __int64)thisStep);
		
		// Add each child variable
		for (size_t j = 0;j < thisStep->nVars;j++) {
			InsertTreeItem(panel, MEASP_STEPTREE, VAL_CHILD, stepTreeItem, VAL_LAST, thisStep->vars[j]->name, NULL, NULL, (unsigned __int64)thisStep->vars[j]);
		}
	}
	
	// Measured nodes
	populateTree(panel, MEASP_OUTVARTREE, measurement->measTree); 
	
	// Dim or undim buttons
	if (measurement->nSteps == 0) {
		SetCtrlAttribute(panel, MEASP_DELETESTEP, ATTR_DIMMED, 1);
		SetCtrlAttribute(panel, MEASP_EDITSTEP, ATTR_DIMMED, 1);
	} else {
		SetCtrlAttribute(panel, MEASP_DELETESTEP, ATTR_DIMMED, 0);
		SetCtrlAttribute(panel, MEASP_EDITSTEP, ATTR_DIMMED, 0);	
	}
	
	if (measurement->measTree->nChildren) {
		SetCtrlAttribute(panel, MEASP_DELETEVAR, ATTR_DIMMED, 0);
	} else {
		SetCtrlAttribute(panel, MEASP_DELETEVAR, ATTR_DIMMED, 1);

	}
}

// Creates a new measurement step and adds it to the specified measurement
// The newly created step is returned
static MeasStep* addMeasurementStep(Measurement* measurement)
{
	// Create the MeasStep with a default variable
	MeasStep* measStep = newMeasStep(measurement);
	newMeasVar(measStep);
	
	// Add to tree box
	updateMeasurementDisplay(measurement);
	
	return measStep;
}

// Removes and deletes the specified measurement step
static void removeMeasurementStep(MeasStep* measStep)
{
	Measurement* measurement = measStep->parent;
	deleteMeasStep(measStep);
	updateMeasurementDisplay(measurement);
}

static void updateSweepDisplay(int panel, MeasVar* measVar)
{
	int ctrlarray = GetCtrlArrayFromResourceID(panel, VARCTRLS);
	
	// If measVar is the null pointer, dim a bunch of stuff and return
	if (measVar == 0) {
		SetCtrlArrayAttribute(ctrlarray, ATTR_DIMMED, 1);
		SetCtrlAttribute(panel, STEPP_DELVAR, ATTR_DIMMED, 1);
		return;
	}
	
	// Un-dim a bunch of stuff
	SetCtrlArrayAttribute(ctrlarray, ATTR_DIMMED, 0);
	SetCtrlAttribute(panel, STEPP_DELVAR, ATTR_DIMMED, 0);
	
	// Fill in values
	SetCtrlVal(panel, STEPP_VARNAME, measVar->name);
	SetCtrlVal(panel, STEPP_COEFF, measVar->coeff);
	SetCtrlVal(panel, STEPP_START, measVar->independentVariable.start);
	SetCtrlVal(panel, STEPP_STEP, measVar->independentVariable.step);
	SetCtrlVal(panel, STEPP_STOP, measVar->independentVariable.start + measVar->independentVariable.step*measVar->parent->nPoints);
}

// Open panel for editing a measurement step
static void editMeasurementVariable(MeasStep* measStep, int index)
{
	// Make sure index is valid
	if (index >= measStep->nVars) {
		index = 0;
	}
	
	// Create a new panel if one doesn't already exist
	if (!measStep->panel) {
		int mainPanel = measStep->parent->zmeasure->panels->main;
		measStep->panel = LoadPanel(mainPanel, "measurement_u.uir", STEPP);

		// Load variable list
		for(size_t i = 0;i < measStep->nVars;i++) {
			InsertListItem(measStep->panel, STEPP_VARS, i, measStep->vars[i]->name, (unsigned __int64)measStep->vars[i]);
		}
		
		updateSweepDisplay(measStep->panel, measStep->vars[index]);
		
		// Store this MeasStep in the panel's callback data
		SetPanelAttribute(measStep->panel, ATTR_CALLBACK_DATA, measStep);
	}
	
	// Display
	DisplayPanel(measStep->panel);
}


static void editMeasurementStep(MeasStep* measStep)
{
	editMeasurementVariable(measStep, 0);
}


/***** Functions for running and controlling the measurement *****/
static void startMeasurement(Measurement* measurement)
{
	// Start the measurement thread. Hold the thread lock for measurement->threadID
	CmtGetLock(measurement->threadLock);
	CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, measurementThread, measurement, &measurement->threadID);
	CmtReleaseLock(measurement->threadLock);
	
	// Dim/undim appropriate ui elements
	setUIState(measurement->panel, MEAS_RUNNING);
}

static void pauseMeasurement(Measurement* measurement)
{
	
	setUIState(measurement->panel, MEAS_PAUSED);
}

static void stopMeasurement(Measurement* measurement)
{
	
	setUIState(measurement->panel, MEAS_STOPPED);
}


static void setUIState(int panel, int state)
{
	int ctrlArray = GetCtrlArrayFromResourceID(panel, SETUP);
	switch (state) {
		case MEAS_RUNNING:
			SetCtrlArrayAttribute(ctrlArray, ATTR_DIMMED, 1);
			break;
		case MEAS_PAUSED:
		case MEAS_STOPPED:
			SetCtrlArrayAttribute(ctrlArray, ATTR_DIMMED, 0);
			SetCtrlAttribute(panel, MEASP_START, ATTR_DIMMED, 0);
			break;
	}
	
	switch (state) {
		case MEAS_RUNNING:
		case MEAS_STOPPED:
			SetCtrlAttribute(panel, MEASP_PAUSE, ATTR_LABEL_TEXT, "Pause");
			break;
		case MEAS_PAUSED:
			SetCtrlAttribute(panel, MEASP_PAUSE, ATTR_LABEL_TEXT, "Resume");
			break;
			
	}
	
	switch (state) {
		case MEAS_RUNNING:
		case MEAS_PAUSED: 
			SetCtrlAttribute(panel, MEASP_PAUSE, ATTR_DIMMED, 0);
			SetCtrlAttribute(panel, MEASP_STOP, ATTR_DIMMED, 0);
			SetCtrlAttribute(panel, MEASP_START, ATTR_DIMMED, 1);
			break;
		case MEAS_STOPPED:
			SetCtrlAttribute(panel, MEASP_PAUSE, ATTR_DIMMED, 1);
			SetCtrlAttribute(panel, MEASP_STOP, ATTR_DIMMED, 1);
			SetCtrlAttribute(panel, MEASP_START, ATTR_DIMMED, 0);
			break;
	}
}

static void populateTreeNode(int panel, int treeControl, struct TreeNode* tree, int parentIndex)
{
	int index = InsertTreeItem(panel, treeControl, VAL_CHILD, parentIndex, VAL_LAST, tree->data, NULL, 0, (unsigned __int64)0);
	SetTreeItemAttribute(panel, treeControl, index, ATTR_CTRL_VAL, (unsigned __int64)tree); 
	SetTreeItemAttribute(panel, treeControl, index, ATTR_COLLAPSED, 1);
	for(size_t i = 0;i < tree->nChildren;i++) {
		populateTreeNode(panel, treeControl, tree->children[i], index);
	}
}

// Fill up a UI tree from the passed TreeNode tree
static void populateTree(int panel, int treeControl, struct TreeNode* tree)
{
	// Delete everything in tree before we start
	DeleteListItem(panel, treeControl, 0, -1);
	
	// Insert each device as a top level item
	for(size_t i = 0;i < tree->nChildren;i++) {
		TreeNode* treeRoot = tree->children[i];
		
		int rootIndex = InsertTreeItem(panel, treeControl, VAL_SIBLING, 0, VAL_LAST, treeRoot->data, NULL, 0, (unsigned __int64)treeRoot); 
		
		TreeNode* retrievedNode;
		GetTreeItemAttribute(panel, TREEP_TREE, rootIndex, ATTR_CTRL_VAL, (unsigned __int64)(&retrievedNode));
		
		for(size_t j = 0;j < treeRoot->nChildren;j++) {
			populateTreeNode(panel, treeControl, treeRoot->children[j], rootIndex);
		}
	}
}


// Measurement thread
int CVICALLBACK measurementThread (void* functionData)
{
	Measurement* measurement = (Measurement*) functionData;
	
	// Delay for testing purposes. Remove later
	Delay(1);
	
	// Establish necessary ZI connections
	
	// Set up the inner sweeper module
	
	// Handle necessary loops for each measurement step
	
	// Set UI back to stopped state. Get the thread lock to safely
	// handle the measurement->panel variable
	CmtGetLock(measurement->threadLock);
	if (measurement->panel) {
		setUIState(measurement->panel, MEAS_STOPPED);
	}
	CmtReleaseLock(measurement->threadLock);
	
	return 0;
}




/***** Functions for allocating and freeing memory *****/

// Raise (or create if necessary) the settings panel for this measurement
void raiseMeasurementPanel(Measurement* measurement)
{
	// We need the thread lock for this entire function to
	// safely use the measurement->panel variable
	CmtGetLock(measurement->threadLock);
	
	// If the panel doesn't exist, make one
	if (!measurement->panel) {
		ZMeasure* zmeasure = measurement->zmeasure;
		measurement->panel = LoadPanel(zmeasure->panels->main, "measurement_u.uir", MEASP);
		SetPanelAttribute(measurement->panel, ATTR_CALLBACK_DATA, measurement);
	}
	
	// Display the panel
	DisplayPanel(measurement->panel);
	
	// Return the thread lock
	CmtReleaseLock(measurement->threadLock);
}

// High level function to create a new measurement in the program
void createMeasurementInUI(ZMeasure* zmeasure)
{
	// Verify we have enough space, do nothing if we don't
	if (zmeasure->measurementCount >= MAX_MEASUREMENTS) {
		MessagePopup("Too Many Measurements", "Can't create new measurement. Too many measurements already defined.");
		return;
	}
	
	// Create Measurement struct
	Measurement* measurement = newMeasurement(zmeasure);
	zmeasure->measurements[zmeasure->measurementCount] = measurement;
	zmeasure->measurementCount += 1;
	
	// Create a new measurement settings panel
	measurement->panel = LoadPanel(zmeasure->panels->main, "measurement_u.uir", MEASP);
	
	// Store the measurement pointer in the panel's callback data
	SetPanelAttribute(measurement->panel, ATTR_CALLBACK_DATA, measurement);
	
	// Update main window tree. Store Measurement* measurement as an int since void* isn't an option
	InsertTreeItem(zmeasure->panels->main, MAINP_MEASUREMENTS, VAL_SIBLING, 0, VAL_LAST, "Untitled Measurement", NULL, NULL, (int)measurement);
	
	// Enable delete button
	SetCtrlAttribute(zmeasure->panels->main, MAINP_DELETEMEAS, ATTR_DIMMED, 0);
	
	// Display the panel
	raiseMeasurementPanel(measurement);
}

// High level function to discard a measurement definition from the program
void deleteMeasurementFromUI(Measurement* measurement) 
{
	ZMeasure* zmeasure = measurement->zmeasure;
	int mainp = zmeasure->panels->main;
	
	// Remove from the tree
	int index;
	GetIndexFromValue(mainp, MAINP_MEASUREMENTS, &index, (int)measurement);
	DeleteListItem(mainp, MAINP_MEASUREMENTS, index, 1);
	
	// Fremove from ZMeasure struct
	removeMeasurementFromZMeasure(zmeasure, measurement);
	
	// Close its panel(s)
	DiscardPanel(measurement->panel);
	
	// Free the memory
	deleteMeasurement(measurement);
	
	// If the tree is empty, disable the delete button
	int count;
	GetNumListItems(mainp, MAINP_MEASUREMENTS, &count);
	if (count == 0) {
		SetCtrlAttribute(mainp, MAINP_DELETEMEAS, ATTR_DIMMED, 1);
	}
}


// High level function to rename a measurement in the UI
void renameMeasurement(Measurement* measurement, char* newname)
{
	// Store the updated value
	ZMeasure* zmeasure = measurement->zmeasure;
	size_t length = strlen(newname);
	char* namecpy = malloc((length + 1) * sizeof(char));
	strcpy(namecpy, newname);
	free(measurement->name);
	measurement->name = namecpy;
	
	// Update the values throughout the UI
	int index;
	GetIndexFromValue(zmeasure->panels->main, MAINP_MEASUREMENTS, &index, (int)measurement);
	SetTreeItemAttribute(zmeasure->panels->main, MAINP_MEASUREMENTS, index, ATTR_LABEL_TEXT, newname);
	
}


/***** Tree selection panel functions *****/
// Open a modal dialog prompting the user to select one or more items from a tree
// parentPanel -> Parent panel to the modal dialog
// tree -> Tree to display
// callback -> Function that will be called when the user selects "okay"
// multiselect -> True to allow user to select more than one item
// leavesOnly -> True to only allow user to select leaves from the tree (nodes without any children)
//
// The callback will not be called if the user cancels the dialog. The callback
// will be sent an array of nodes and a length of this array. The user is
// responsible to free the array when it is no longer needed. 
int treeSelectPopup(int parentPanel, TreeNode* tree, void (*callback)(void* callbackdata, TreeNode** nodeList), void* callbackdata, int selectionType, int leavesOnly)
{
	// Load variable selection panel
	int panel = LoadPanel(parentPanel, "measurement_u.uir", TREEP);
	
	// Fill in the tree with the measurable variables
	populateTree(panel, TREEP_TREE, tree);
	
	// Set multiselect
	SetCtrlAttribute(panel, TREEP_TREE, ATTR_SELECTION_MODE, selectionType);
	
	// Store callback parameters
	struct TreeSelectParams *params = malloc(sizeof(struct TreeSelectParams));
	params->function = callback;
	params->data = callbackdata;
	params->leavesOnly = leavesOnly;
	SetPanelAttribute(panel, ATTR_CALLBACK_DATA, params); 
	
	// Create modal dialog
	InstallPopup(panel);
	
	return panel;
}


int CVICALLBACK treepanel_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)
	{
		// Get the callback parameters
		struct TreeSelectParams *params;
		GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &params);
		
		switch (control) {
			case TREEP_CANCEL:
				break;
			case TREEP_OKAY:  // Fall through
			case TREEP_TREE:
				// Determine selected items
				TreeNode** selectedItems = getTreeSelectedItems(panel, TREEP_TREE);
				
				// Call callback
				void (*callback)(void* callbackdata, TreeNode* nodeList) = params->function;
				(*callback)(params->data, selectedItems);
				
				free(selectedItems);
				
				break;
		}
		
		// In any case, close the panel and free the memory
		RemovePopup(0);
		free(params);
		params = 0;
	}
	return 0;
}

// Returns a null terminated array of the selected tree items
// The calling function is responsible for freeing this array when
// it is no longer needed.
static TreeNode** getTreeSelectedItems(int panel, int control)
{
	int error, index;
	
	// Count selected items
	size_t itemCount = 0; 
	for (error = GetTreeItem(panel, control, VAL_ALL, 0, VAL_FIRST, VAL_NEXT_PLUS_SELF, VAL_SELECTED, &index);
         index >= 0 && error >= 0;
         error = GetTreeItem(panel, control, VAL_ALL, 0, index, VAL_NEXT, VAL_SELECTED, &index))
    {
		itemCount++;
    }
	
	// Allocate a null terminated array
	TreeNode **treeList = malloc((itemCount+1) * sizeof(TreeNode*));
	treeList[itemCount] = 0;	
	
	// Accumulate selected items
	size_t itemIndex = 0;
	for (error = GetTreeItem(panel, control, VAL_ALL, 0, VAL_FIRST, VAL_NEXT_PLUS_SELF, VAL_SELECTED, &index);
         index >= 0 && error >= 0;
         error = GetTreeItem(panel, control, VAL_ALL, 0, index, VAL_NEXT, VAL_SELECTED, &index))
    {
		GetTreeItemAttribute(panel, TREEP_TREE, index, ATTR_CTRL_VAL, (unsigned __int64*)(treeList + itemIndex));
		itemIndex++;
    }

	
	// TODO: Filter for leaves only if chosen
	//struct TreeSelectParams* params;
	//GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &params);
	
	return treeList;
}


/***** Callback Functions *****/
int CVICALLBACK measpanel_CB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_KEYPRESS)
		switch(eventData1) {
			case VAL_ESC_VKEY:
				Measurement* measurement = callbackData;
				
				// Hide the panel
				HidePanel(measurement->panel);
	}
	return 0;
}


// Modal dialog panel
int CVICALLBACK popuppanel_CB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_KEYPRESS)
		switch(eventData1) {
			case VAL_ESC_VKEY:
				RemovePopup(0);	// Remove active popup (this panel)
				return 1;		// Swallow callback
	}
	
	if (event == EVENT_CLOSE) {
		RemovePopup(0);
		return 0;
	}
	
	return 0;
}


// Panel should destroy itself on esc
int CVICALLBACK steppanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_KEYPRESS)
		switch(eventData1) {
			case VAL_ESC_VKEY:
				MeasStep* measStep;
				GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measStep);
				measStep->panel = 0;
				DiscardPanel(panel);
				return 1;		// Swallow callback
	}
	
	return 0;
}


int CVICALLBACK startstop_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT) {
		Measurement* measurement;
		GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measurement);
		switch (control) {
			case MEASP_START:
				startMeasurement(measurement);
				break;
			case MEASP_PAUSE:
				pauseMeasurement(measurement);
				break;
			case MEASP_STOP:
				stopMeasurement(measurement);
				break;
		}
	}
	return 0;
}

int CVICALLBACK vars_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)
	{   
		Measurement* measurement;
		GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measurement);
		switch (control) {
			case MEASP_ADDVAR:
				treeSelectPopup(panel, measNodes, outVarsSelect_CB, measurement, VAL_SELECTION_MULTIPLE, 1);
				break;
			case MEASP_DELETEVAR:
				TreeNode* measNode;
				GetCtrlVal(panel, MEASP_OUTVARTREE, (unsigned __int64*)(&measNode));
				char path[MAX_PATH_LEN];
				getPathFromTree(measNode, path);
				removePathFromTree(measurement->measTree, path);
				updateMeasurementDisplay(measurement);
				break;
		}
	}
	return 0;
}

void addPathToTreeIterFcn(TreeNode* node, size_t depth, TreeNode* measNodes)
{   
	char* path[MAX_PATH_LEN];
	getPathFromTree(node, path);
	addPathToTree(measNodes, path);
}

// Passed to treeSelectPopup to handle selecting output variable nodes
static void outVarsSelect_CB(Measurement* measurement, TreeNode** nodeList)
{
	if (nodeList) {
		TreeNode* thisNode;
		while((thisNode = *nodeList++)) {
			depthFirstIterTree(thisNode, 0, measurement->measTree, addPathToTreeIterFcn);
		}
		updateMeasurementDisplay(measurement); 	
	}
}



int CVICALLBACK renameMeas_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)
	{
		Measurement* measurement;
		GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measurement);
		
		int length;
		GetCtrlAttribute(panel, MEASP_NAME, ATTR_STRING_TEXT_LENGTH, &length);
		
		char* newname = malloc((length+1) * sizeof(char));
		GetCtrlVal(panel, MEASP_NAME, newname);
		
		renameMeasurement(measurement, newname);
		
		free(newname);
	}
	return 0;
}

// Called for buttons to add, delete, or edit measurement steps
int CVICALLBACK steps_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)
	{
		Measurement* measurement;
		GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measurement);
		int ancestor;
		int itemIndex;
		MeasStep* measStep;
		
		switch (control) {
			case MEASP_ADDSTEP:
				addMeasurementStep(measurement);
				break;
			case MEASP_DELETESTEP:
				GetActiveTreeItem(panel, MEASP_STEPTREE, &itemIndex);
				GetTreeItem(panel, MEASP_STEPTREE, VAL_ANCESTOR, itemIndex, VAL_LAST, VAL_NEXT_PLUS_SELF, 0, &ancestor);
				if (ancestor == -1) {
					GetCtrlVal(panel, MEASP_STEPTREE, (int*)(&measStep));
				} else {
					GetTreeItemAttribute(panel, MEASP_STEPTREE, ancestor, ATTR_CTRL_VAL, (int*)(&measStep));
				}
				removeMeasurementStep(measStep);
				break;
			case MEASP_STEPTREE:
			case MEASP_EDITSTEP:
				GetActiveTreeItem(panel, MEASP_STEPTREE, &itemIndex);
				GetTreeItem(panel, MEASP_STEPTREE, VAL_ANCESTOR, itemIndex, VAL_LAST, VAL_NEXT_PLUS_SELF, 0, &ancestor);
				if (ancestor == -1) {
					GetCtrlVal(panel, MEASP_STEPTREE, (int*)(&measStep));
				} else {
					GetTreeItemAttribute(panel, MEASP_STEPTREE, ancestor, ATTR_CTRL_VAL, (unsigned __int64*)(&measStep));
				}
				editMeasurementStep(measStep);
				break;
		}
	}
	return 0;
}

static void nodeSelect_CB(void* callbackdata, TreeNode** nodeList) 
{
	char* path = nodeList[0]->data;
}

int CVICALLBACK selectPath_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			MeasStep* measStep;
			GetPanelAttribute(panel, ATTR_CALLBACK_DATA, &measStep);
			
			treeSelectPopup(panel, varNodes, nodeSelect_CB, measStep, 0, 1);
			break;
	}
	return 0;
}

int CVICALLBACK measvar_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)
	{
		MeasStep* measStep;
		
	}
	return 0;
}
