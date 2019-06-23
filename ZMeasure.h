/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  ABOUTP                           1       /* callback function: subpanel_CB */
#define  ABOUTP_CLOSEBUTTON               2       /* control type: command, callback function: closePanel_CB */
#define  ABOUTP_TEXTMSG                   3       /* control type: textMsg, callback function: (none) */
#define  ABOUTP_VERSIONNUM                4       /* control type: textMsg, callback function: (none) */
#define  ABOUTP_TEXTMSG_3                 5       /* control type: textMsg, callback function: (none) */
#define  ABOUTP_TEXTMSG_2                 6       /* control type: textMsg, callback function: (none) */

#define  MAINP                            2       /* callback function: mainPanel_CB */

#define  ZCONNP                           3       /* callback function: subpanel_CB */
#define  ZCONNP_DEVICE                    2       /* control type: string, callback function: (none) */
#define  ZCONNP_SERVER                    3       /* control type: string, callback function: (none) */
#define  ZCONNP_PORT                      4       /* control type: numeric, callback function: (none) */
#define  ZCONNP_CANCEL                    5       /* control type: command, callback function: closePanel_CB */
#define  ZCONNP_CONNECT                   6       /* control type: command, callback function: connect_CB */

#define  ZNODESP                          4       /* callback function: subpanel_CB */
#define  ZNODESP_SETTINGTREE              2       /* control type: tree, callback function: nodeTree_CB */
#define  ZNODESP_NEWCONNECTION            3       /* control type: command, callback function: manageConnections_CB */
#define  ZNODESP_DISCONNECT               4       /* control type: command, callback function: manageConnections_CB */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_FILE                     2
#define  MENUBAR_FILE_SAVE                3
#define  MENUBAR_FILE_EXIT                4       /* callback function: exit_CB */
#define  MENUBAR_SETUP                    5
#define  MENUBAR_SETUP_ZURICH             6       /* callback function: openPanel_CB */
#define  MENUBAR_SETUP_SWEEP              7       /* callback function: openPanel_CB */
#define  MENUBAR_SETUP_OUTPUT             8       /* callback function: openPanel_CB */
#define  MENUBAR_START                    9
#define  MENUBAR_HELP                     10
#define  MENUBAR_HELP_ABOUT               11      /* callback function: openPanel_CB */


     /* Callback Prototypes: */

int  CVICALLBACK closePanel_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK connect_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK exit_CB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK mainPanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK manageConnections_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK nodeTree_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK openPanel_CB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK subpanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
