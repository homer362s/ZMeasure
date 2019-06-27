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
#define  MAINP_SPLITTER                   2       /* control type: splitter, callback function: (none) */
#define  MAINP_DISCONNECT                 3       /* control type: command, callback function: (none) */
#define  MAINP_CONNECT                    4       /* control type: command, callback function: (none) */
#define  MAINP_CONNECTIONS                5       /* control type: listBox, callback function: (none) */

#define  MEASVARSP                        3       /* callback function: subpanel_CB */
#define  MEASVARSP_OUTVARTREE             2       /* control type: tree, callback function: (none) */
#define  MEASVARSP_INVARTREE              3       /* control type: tree, callback function: (none) */
#define  MEASVARSP_REMOVE                 4       /* control type: command, callback function: (none) */
#define  MEASVARSP_START                  5       /* control type: command, callback function: startstop_CB */
#define  MEASVARSP_FILEPREFIX             6       /* control type: string, callback function: (none) */
#define  MEASVARSP_STOP                   7       /* control type: command, callback function: startstop_CB */
#define  MEASVARSP_PAUSE                  8       /* control type: command, callback function: startstop_CB */

#define  ZCONNP                           4       /* callback function: subpanel_CB */
#define  ZCONNP_DEVICE                    2       /* control type: string, callback function: (none) */
#define  ZCONNP_SERVER                    3       /* control type: string, callback function: (none) */
#define  ZCONNP_PORT                      4       /* control type: numeric, callback function: (none) */
#define  ZCONNP_CANCEL                    5       /* control type: command, callback function: closePanel_CB */
#define  ZCONNP_CONNECT                   6       /* control type: command, callback function: connect_CB */

#define  ZNODESP                          5       /* callback function: subpanel_CB */
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
#define  MENUBAR_SETUP_MEASUREMENT        7       /* callback function: openPanel_CB */
#define  MENUBAR_HELP                     8
#define  MENUBAR_HELP_ABOUT               9       /* callback function: openPanel_CB */


     /* Callback Prototypes: */

int  CVICALLBACK closePanel_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK connect_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK exit_CB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK mainPanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK manageConnections_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK nodeTree_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK openPanel_CB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK startstop_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK subpanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
