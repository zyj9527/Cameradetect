//
// Created by shinco on 15-12-24.
//

#ifndef GSTREAMER_COMMON_H
#define GSTREAMER_COMMON_H
#include <gst/gst.h>
#include "shincoDbus.h"
#include "shellDbus.h"
#define TIMEOUT 10

#define WM_CARPLAY_CTRL_BORROW_SCREEN_NEVER	(3000+16)
#define WM_CARPLAY_CTRL_UNBORROW_SCREEN 	(3000+17)
#define WM_CARPLAY_CONNECTED                (3100+1)
#define WM_CARPLAY_DISCONNECTED             (3100+2)
#define WM_CARPLAY_CTRL_BORROWACK           (3200+20)
#define WM_CARPLAY_CTRL_UNBORROWACK         (3200+21)

#define IPC_SERVICE_ID                      (100)
#define IPC_NAVI_ID                         (101)
#define IPC_APPLINK_ID                      (102)
#define IPC_POPWIND_ID                      (103)
#define IPC_CAMERA_ID                       (104)



typedef void (*VoidFunc) (gpointer data);
/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData {
    GstElement *playbin;  /* Our one and only element */
    gboolean playing;      /* Are we in the PLAYING state? */
    GMainLoop *loop;
    gboolean isplaydone;
	
    gchar isReverse;
    gboolean isManualReverse;
    gint angle;
    gchar language;
	gboolean needRunLoop;
	
	/* Carplay element*/
	gboolean carplayConnected;
	gboolean carplayPlaying;
	gboolean carplayBorrowed;
	
    VoidFunc startPlay;
    VoidFunc stopPlay;
    SerialPortInterface *serialPortInterface;
    ShnInterface *shellInterface;

    GThreadFunc radarThread;

} CustomData;

#endif //GSTREAMER_COMMON_H
