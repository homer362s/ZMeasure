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

#define  MEASP                            1       /* callback function: measpanel_CB */
#define  MEASP_OUTVARTREE                 2       /* control type: tree, callback function: (none) */
#define  MEASP_STEPTREE                   3       /* control type: tree, callback function: steps_CB */
#define  MEASP_FULLNODETREE               4       /* control type: tree, callback function: (none) */
#define  MEASP_NAME                       5       /* control type: string, callback function: renameMeas_CB */
#define  MEASP_STOP                       6       /* control type: command, callback function: startstop_CB */
#define  MEASP_PAUSE                      7       /* control type: command, callback function: startstop_CB */
#define  MEASP_START                      8       /* control type: command, callback function: startstop_CB */
#define  MEASP_DECORATION                 9       /* control type: deco, callback function: (none) */
#define  MEASP_TEXTMSG                    10      /* control type: textMsg, callback function: (none) */
#define  MEASP_ADDVAR                     11      /* control type: command, callback function: vars_CB */
#define  MEASP_DELETEVAR                  12      /* control type: command, callback function: vars_CB */
#define  MEASP_EDITSTEP                   13      /* control type: command, callback function: steps_CB */
#define  MEASP_DELETESTEP                 14      /* control type: command, callback function: steps_CB */
#define  MEASP_ADDSTEP                    15      /* control type: command, callback function: steps_CB */

#define  STEPP                            2       /* callback function: steppanel_CB */
#define  STEPP_VARS                       2       /* control type: listBox, callback function: (none) */
#define  STEPP_STEP                       3       /* control type: numeric, callback function: (none) */
#define  STEPP_STOP                       4       /* control type: numeric, callback function: (none) */
#define  STEPP_NPOINTS                    5       /* control type: numeric, callback function: (none) */
#define  STEPP_COEFF                      6       /* control type: numeric, callback function: (none) */
#define  STEPP_START                      7       /* control type: numeric, callback function: (none) */
#define  STEPP_STEPNAME                   8       /* control type: string, callback function: (none) */
#define  STEPP_PATH                       9       /* control type: string, callback function: (none) */
#define  STEPP_LOADPATH                   10      /* control type: command, callback function: selectPath_CB */
#define  STEPP_VARNAME                    11      /* control type: string, callback function: (none) */
#define  STEPP_DELVAR                     12      /* control type: command, callback function: (none) */
#define  STEPP_ADDVAR                     13      /* control type: command, callback function: (none) */
#define  STEPP_DECORATION                 14      /* control type: deco, callback function: (none) */

#define  TREEP                            3       /* callback function: popuppanel_CB */
#define  TREEP_TREE                       2       /* control type: tree, callback function: treepanel_CB */
#define  TREEP_CANCEL                     3       /* control type: command, callback function: treepanel_CB */
#define  TREEP_OKAY                       4       /* control type: command, callback function: treepanel_CB */


     /* Control Arrays: */

#define  SETUP                            1
#define  VARCTRLS                         2

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK measpanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK popuppanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK renameMeas_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK selectPath_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK startstop_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK steppanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK steps_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK treepanel_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK vars_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
