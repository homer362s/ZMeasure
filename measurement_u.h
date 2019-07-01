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
#define  MEASP_STEPTREE                   3       /* control type: tree, callback function: (none) */
#define  MEASP_FULLNODETREE               4       /* control type: tree, callback function: (none) */
#define  MEASP_NAME                       5       /* control type: string, callback function: (none) */
#define  MEASP_STOP                       6       /* control type: command, callback function: startstop_CB */
#define  MEASP_PAUSE                      7       /* control type: command, callback function: startstop_CB */
#define  MEASP_START                      8       /* control type: command, callback function: startstop_CB */
#define  MEASP_DECORATION                 9       /* control type: deco, callback function: (none) */
#define  MEASP_TEXTMSG                    10      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

#define  SETUP                            1

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK measpanel_CB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK startstop_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
