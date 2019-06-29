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
#define  MAINP_DISCONNECT                 3       /* control type: command, callback function: manageConnections_CB */
#define  MAINP_CONNECT                    4       /* control type: command, callback function: manageConnections_CB */
#define  MAINP_CONNECTIONS                5       /* control type: listBox, callback function: (none) */
#define  MAINP_FREQ_8                     6       /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_7                     7       /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_6                     8       /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_5                     9       /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_4                     10      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_3                     11      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_2                     12      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_1                     13      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_HARM_8                     14      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_HARM_7                     15      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_8                16      /* control type: command, callback function: setZIValue_CB */
#define  MAINP_PHASE_8                    17      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_SIG_8                      18      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ORDER_8                    19      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_6                     20      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_7                21      /* control type: command, callback function: setZIValue_CB */
#define  MAINP_PHASE_7                    22      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_SIG_7                      23      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ORDER_7                    24      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_5                     25      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_6                26      /* control type: command, callback function: setZIValue_CB */
#define  MAINP_PHASE_6                    27      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_SIG_6                      28      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ORDER_6                    29      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_4                     30      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_5                31      /* control type: command, callback function: setZIValue_CB */
#define  MAINP_PHASE_5                    32      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_SIG_5                      33      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ORDER_5                    34      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_3                     35      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_4                36      /* control type: command, callback function: setZIValue_CB */
#define  MAINP_PHASE_4                    37      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_SIG_4                      38      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ORDER_4                    39      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_2                     40      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_OSC_8                      41      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_7                      42      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_6                      43      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_5                      44      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_4                      45      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_3                      46      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_2                      47      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_1                      48      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_3                49      /* control type: command, callback function: setZIValue_CB */
#define  MAINP_PHASE_3                    50      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_SIG_3                      51      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ORDER_3                    52      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_1                     53      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_FREQLABEL                  54      /* control type: textMsg, callback function: (none) */
#define  MAINP_AUTOPHASE_2                55      /* control type: command, callback function: setZIValue_CB */
#define  MAINP_PHASE_2                    56      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_SIG_2                      57      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ORDER_2                    58      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARMLABEL                  59      /* control type: textMsg, callback function: (none) */
#define  MAINP_AUTOPHASE_1                60      /* control type: command, callback function: setZIValue_CB */
#define  MAINP_PHASE_1                    61      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_SIG_1                      62      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ORDER_1                    63      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_PHASELABEL                 64      /* control type: textMsg, callback function: (none) */
#define  MAINP_INPUTLABEL                 65      /* control type: textMsg, callback function: (none) */
#define  MAINP_ORDERLABEL                 66      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL2_1                67      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL2_2                68      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL2_3                69      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL2_4                70      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL2_5                71      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL2_7                72      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL2_6                73      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL_1                 74      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL_2                 75      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL_3                 76      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL_4                 77      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL_5                 78      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL_7                 79      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL_6                 80      /* control type: textMsg, callback function: (none) */
#define  MAINP_FREQLOCK_8                 81      /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_7                 82      /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_6                 83      /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_5                 84      /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_4                 85      /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_3                 86      /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_2                 87      /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_1                 88      /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_NODES                      89      /* control type: tree, callback function: (none) */
#define  MAINP_SPLITTER_2                 90      /* control type: splitter, callback function: (none) */
#define  MAINP_ROWLABEL_8                 91      /* control type: textMsg, callback function: (none) */
#define  MAINP_ROWLABEL2_8                92      /* control type: textMsg, callback function: (none) */
#define  MAINP_TEXTMSG_2                  93      /* control type: textMsg, callback function: (none) */
#define  MAINP_TEXTMSG                    94      /* control type: textMsg, callback function: (none) */
#define  MAINP_DECORATION_3               95      /* control type: deco, callback function: (none) */
#define  MAINP_DECORATION_2               96      /* control type: deco, callback function: (none) */
#define  MAINP_OSCLABEL                   97      /* control type: textMsg, callback function: (none) */

#define  MEASVARSP                        3       /* callback function: subpanel_CB */
#define  MEASVARSP_OUTVARTREE             2       /* control type: tree, callback function: (none) */
#define  MEASVARSP_INVARTREE              3       /* control type: tree, callback function: (none) */
#define  MEASVARSP_REMOVE                 4       /* control type: command, callback function: (none) */
#define  MEASVARSP_START                  5       /* control type: command, callback function: startstop_CB */
#define  MEASVARSP_FILEPREFIX             6       /* control type: string, callback function: (none) */
#define  MEASVARSP_STOP                   7       /* control type: command, callback function: startstop_CB */
#define  MEASVARSP_PAUSE                  8       /* control type: command, callback function: startstop_CB */

#define  NEWZCONNP                        4       /* callback function: subpanel_CB */
#define  NEWZCONNP_DEVICE                 2       /* control type: string, callback function: (none) */
#define  NEWZCONNP_SERVER                 3       /* control type: string, callback function: (none) */
#define  NEWZCONNP_PORT                   4       /* control type: numeric, callback function: (none) */
#define  NEWZCONNP_CANCEL                 5       /* control type: command, callback function: closePanel_CB */
#define  NEWZCONNP_CONNECT                6       /* control type: command, callback function: connect_CB */

#define  ZNODESP                          5       /* callback function: subpanel_CB */
#define  ZNODESP_SETTINGTREE              2       /* control type: tree, callback function: nodeTree_CB */
#define  ZNODESP_NEWCONNECTION            3       /* control type: command, callback function: manageConnections_CB */
#define  ZNODESP_DISCONNECT               4       /* control type: command, callback function: manageConnections_CB */


     /* Control Arrays: */

#define  AUTOPHASE                        1
#define  FREQ                             2
#define  FREQLOCK                         3
#define  HARM                             4
#define  ORDER                            5
#define  OSCS                             6
#define  PHASE                            7
#define  SIG                              8

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
int  CVICALLBACK setZIValue_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK startstop_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK subpanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
