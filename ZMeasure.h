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
#define  MAINP_NEWMEAS                    2       /* control type: command, callback function: manageMeasurements_CB */
#define  MAINP_CONNECT                    3       /* control type: command, callback function: manageConnections_CB */
#define  MAINP_CONNECTIONS                4       /* control type: listBox, callback function: (none) */
#define  MAINP_MEASUREMENTS               5       /* control type: tree, callback function: (none) */
#define  MAINP_FREQ_1                     6       /* control type: string, callback function: setZIValue_CB */
#define  MAINP_OSC_1                      7       /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_1                     8       /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_PHASE_1                    9       /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_1                10      /* control type: command, callback function: autophase_CB */
#define  MAINP_SIG_1                      11      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ORDER_1                    12      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_ENABLE_1                   13      /* control type: LED, callback function: enableDemod_CB */
#define  MAINP_RATE_1                     14      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP1_1                     15      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP1ENABLE_1               16      /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_AMP2_1                     17      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP2ENABLE_1               18      /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_DECORATION_2               19      /* control type: deco, callback function: (none) */
#define  MAINP_DELETEMEAS                 20      /* control type: command, callback function: manageMeasurements_CB */
#define  MAINP_SPLITTER                   21      /* control type: splitter, callback function: (none) */
#define  MAINP_DISCONNECT                 22      /* control type: command, callback function: manageConnections_CB */
#define  MAINP_FREQ_8                     23      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_7                     24      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_6                     25      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_5                     26      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_4                     27      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_FREQ_3                     28      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_DECORATION_3               29      /* control type: deco, callback function: (none) */
#define  MAINP_FREQ_2                     30      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP2_8                     31      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP1_8                     32      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP2_7                     33      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP1_7                     34      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP2_6                     35      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP1_6                     36      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP2_5                     37      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP1_5                     38      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP2_4                     39      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP1_4                     40      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP2_3                     41      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP1_3                     42      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP2_2                     43      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_AMP1_2                     44      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_RATE_8                     45      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_RATE_7                     46      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_RATE_6                     47      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_RATE_5                     48      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_RATE_4                     49      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_RATE_3                     50      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_RATE_2                     51      /* control type: string, callback function: setZIValue_CB */
#define  MAINP_HARM_8                     52      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_HARM_7                     53      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_8                54      /* control type: command, callback function: autophase_CB */
#define  MAINP_PHASE_8                    55      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_ORDER_8                    56      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_6                     57      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_7                58      /* control type: command, callback function: autophase_CB */
#define  MAINP_PHASE_7                    59      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_ORDER_7                    60      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_5                     61      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_6                62      /* control type: command, callback function: autophase_CB */
#define  MAINP_PHASE_6                    63      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_ORDER_6                    64      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_4                     65      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_5                66      /* control type: command, callback function: autophase_CB */
#define  MAINP_PHASE_5                    67      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_ORDER_5                    68      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_3                     69      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_4                70      /* control type: command, callback function: autophase_CB */
#define  MAINP_PHASE_4                    71      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_ORDER_4                    72      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARM_2                     73      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_OSC_8                      74      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_7                      75      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_6                      76      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_5                      77      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_4                      78      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_3                      79      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_OSC_2                      80      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_AUTOPHASE_3                81      /* control type: command, callback function: autophase_CB */
#define  MAINP_PHASE_3                    82      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_ORDER_3                    83      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_FREQLABEL                  84      /* control type: textMsg, callback function: (none) */
#define  MAINP_AUTOPHASE_2                85      /* control type: command, callback function: autophase_CB */
#define  MAINP_PHASE_2                    86      /* control type: numeric, callback function: setZIValue_CB */
#define  MAINP_ORDER_2                    87      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_HARMLABEL                  88      /* control type: textMsg, callback function: (none) */
#define  MAINP_SIG_8                      89      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_SIG_7                      90      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_SIG_6                      91      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_SIG_5                      92      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_SIG_4                      93      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_SIG_3                      94      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_SIG_2                      95      /* control type: ring, callback function: setZIValue_CB */
#define  MAINP_PHASELABEL                 96      /* control type: textMsg, callback function: (none) */
#define  MAINP_INPUTLABEL                 97      /* control type: textMsg, callback function: (none) */
#define  MAINP_ORDERLABEL                 98      /* control type: textMsg, callback function: (none) */
#define  MAINP_DEMODROWLABEL_8            99      /* control type: textMsg, callback function: (none) */
#define  MAINP_OSCROWLABEL_8              100     /* control type: textMsg, callback function: (none) */
#define  MAINP_DEMODROWLABEL_7            101     /* control type: textMsg, callback function: (none) */
#define  MAINP_OSCROWLABEL_7              102     /* control type: textMsg, callback function: (none) */
#define  MAINP_DEMODROWLABEL_6            103     /* control type: textMsg, callback function: (none) */
#define  MAINP_OSCROWLABEL_6              104     /* control type: textMsg, callback function: (none) */
#define  MAINP_DEMODROWLABEL_5            105     /* control type: textMsg, callback function: (none) */
#define  MAINP_OSCROWLABEL_5              106     /* control type: textMsg, callback function: (none) */
#define  MAINP_DEMODROWLABEL_4            107     /* control type: textMsg, callback function: (none) */
#define  MAINP_OSCROWLABEL_4              108     /* control type: textMsg, callback function: (none) */
#define  MAINP_DEMODROWLABEL_3            109     /* control type: textMsg, callback function: (none) */
#define  MAINP_OSCROWLABEL_3              110     /* control type: textMsg, callback function: (none) */
#define  MAINP_DEMODROWLABEL_2            111     /* control type: textMsg, callback function: (none) */
#define  MAINP_OSCROWLABEL_2              112     /* control type: textMsg, callback function: (none) */
#define  MAINP_DEMODROWLABEL_1            113     /* control type: textMsg, callback function: (none) */
#define  MAINP_OSCROWLABEL_1              114     /* control type: textMsg, callback function: (none) */
#define  MAINP_FREQLOCK_8                 115     /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_7                 116     /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_6                 117     /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_5                 118     /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_4                 119     /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_AMP2ENABLE_8               120     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_AMP1ENABLE_8               121     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_FREQLOCK_3                 122     /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_FREQLOCK_2                 123     /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_AMP2ENABLE_7               124     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_AMP1ENABLE_7               125     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_FREQLOCK_1                 126     /* control type: LED, callback function: setZIValue_CB */
#define  MAINP_AMP2ENABLE_6               127     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_AMP1ENABLE_6               128     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_SPLITTER_2                 129     /* control type: splitter, callback function: (none) */
#define  MAINP_TEXTMSG_2                  130     /* control type: textMsg, callback function: (none) */
#define  MAINP_AMP2ENABLE_5               131     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_AMP1ENABLE_5               132     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_TEXTMSG                    133     /* control type: textMsg, callback function: (none) */
#define  MAINP_ENABLE_8                   134     /* control type: LED, callback function: enableDemod_CB */
#define  MAINP_AMP2ENABLE_4               135     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_AMP1ENABLE_4               136     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_ENABLE_7                   137     /* control type: LED, callback function: enableDemod_CB */
#define  MAINP_ENABLE_6                   138     /* control type: LED, callback function: enableDemod_CB */
#define  MAINP_AMP2ENABLE_3               139     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_AMP1ENABLE_3               140     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_ENABLE_5                   141     /* control type: LED, callback function: enableDemod_CB */
#define  MAINP_ENABLE_4                   142     /* control type: LED, callback function: enableDemod_CB */
#define  MAINP_AMP2ENABLE_2               143     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_AMP1ENABLE_2               144     /* control type: LED, callback function: enableOutput_CB */
#define  MAINP_ENABLE_3                   145     /* control type: LED, callback function: enableDemod_CB */
#define  MAINP_ENABLE_2                   146     /* control type: LED, callback function: enableDemod_CB */
#define  MAINP_OSCLABEL                   147     /* control type: textMsg, callback function: (none) */
#define  MAINP_ENABLELABEL                148     /* control type: textMsg, callback function: (none) */
#define  MAINP_RATELABEL                  149     /* control type: textMsg, callback function: (none) */
#define  MAINP_DECORATION_4               150     /* control type: deco, callback function: (none) */
#define  MAINP_TEXTMSG_3                  151     /* control type: textMsg, callback function: (none) */
#define  MAINP_OUTROWLABEL_1              152     /* control type: textMsg, callback function: (none) */
#define  MAINP_OUTROWLABEL_2              153     /* control type: textMsg, callback function: (none) */
#define  MAINP_OUTROWLABEL_3              154     /* control type: textMsg, callback function: (none) */
#define  MAINP_OUTROWLABEL_4              155     /* control type: textMsg, callback function: (none) */
#define  MAINP_OUTROWLABEL_5              156     /* control type: textMsg, callback function: (none) */
#define  MAINP_OUTROWLABEL_6              157     /* control type: textMsg, callback function: (none) */
#define  MAINP_OUTROWLABEL_7              158     /* control type: textMsg, callback function: (none) */
#define  MAINP_OUTROWLABEL_8              159     /* control type: textMsg, callback function: (none) */
#define  MAINP_ENABLELABEL_3              160     /* control type: textMsg, callback function: (none) */
#define  MAINP_ENABLELABEL_2              161     /* control type: textMsg, callback function: (none) */
#define  MAINP_AMP1LABEL                  162     /* control type: textMsg, callback function: (none) */
#define  MAINP_AMP2LABEL                  163     /* control type: textMsg, callback function: (none) */
#define  MAINP_STRING                     164     /* control type: string, callback function: (none) */

#define  NEWZCONNP                        3       /* callback function: subpanel_CB */
#define  NEWZCONNP_CONNECT                2       /* control type: command, callback function: connect_CB */
#define  NEWZCONNP_DEVICE                 3       /* control type: string, callback function: (none) */
#define  NEWZCONNP_SERVER                 4       /* control type: string, callback function: (none) */
#define  NEWZCONNP_PORT                   5       /* control type: numeric, callback function: (none) */
#define  NEWZCONNP_CANCEL                 6       /* control type: command, callback function: closePanel_CB */


     /* Control Arrays: */

#define  AUTOPHASE                        1
#define  ENABLE                           2
#define  FREQ                             3
#define  FREQLOCK                         4
#define  HARM                             5
#define  ORDER                            6
#define  OSCS                             7
#define  PHASE                            8
#define  SIG                              9

     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_FILE                     2
#define  MENUBAR_FILE_SAVE                3
#define  MENUBAR_FILE_EXIT                4       /* callback function: exit_CB */
#define  MENUBAR_SETUP                    5
#define  MENUBAR_SETUP_CONNECTION         6
#define  MENUBAR_SETUP_CONNECTION_SUBMENU 7
#define  MENUBAR_SETUP_CONNECTION_NEW     8       /* callback function: openPanel_CB */
#define  MENUBAR_SETUP_CONNECTION_DELETE  9
#define  MENUBAR_SETUP_MEASUREMENT        10      /* callback function: openPanel_CB */
#define  MENUBAR_SETUP_MEASUREMENT_SUBMENU 11
#define  MENUBAR_SETUP_MEASUREMENT_NEW    12
#define  MENUBAR_SETUP_MEASUREMENT_EDIT   13
#define  MENUBAR_SETUP_MEASUREMENT_DELETE 14
#define  MENUBAR_SETUP_REFRESH            15
#define  MENUBAR_SETUP_REFRESH_SUBMENU    16
#define  MENUBAR_SETUP_REFRESH_AUTOREFRESH 17
#define  MENUBAR_SETUP_REFRESH_MANUALREFRESH 18
#define  MENUBAR_HELP                     19
#define  MENUBAR_HELP_ABOUT               20      /* callback function: openPanel_CB */


     /* Callback Prototypes: */

int  CVICALLBACK autophase_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK closePanel_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK connect_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK enableDemod_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK enableOutput_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK exit_CB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK mainPanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK manageConnections_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK manageMeasurements_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK openPanel_CB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK setZIValue_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK subpanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
