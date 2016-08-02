//
// Created by shinco on 15-12-24.
//

#ifndef GSTREAMER_CAMERA_H
#define GSTREAMER_CAMERA_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <linux/mxcfb.h>
#include <linux/ipu.h>
#include <linux/v4l2-controls.h>
#include <linux/mxc_v4l2.h>


#include <png.h>
#include <zlib.h>
#include <png.h>
#include "common.h"

#define TFAIL -1

#define V4L_CAPTURE_DEV "/dev/video0"
#define V4L_OUTPUT_DEV "/dev/video17"
#define FB_DEV			"/dev/fb1"
#define DATADIR        "/usr/share/radar"
#define DISPLAY_FORMAT	v4l2_fourcc('A', 'B', 'G', 'R')
#define BYTE_PER_PIXEL	4

#define START_X_TRACK		5
#define START_Y_TRACK		162

#define START_X_BG		0
#define START_Y_BG		0

#define START_X_LOGO_CAR        720
#define START_Y_LOGO_CAR        60

#define NUM_OF_PATH 	91

struct marker_t {
    int startx;
    int starty;
    png_image image;
    png_bytep buffer;
    char *colormap;
};


struct testbuffer
{
    unsigned char *start;
    size_t offset;
    unsigned int length;
};

void Camerainit(CustomData*);
gpointer startRadar(gpointer arg);
gpointer startRadar360(gpointer arg);
void load_bg(CustomData *data);

#endif //GSTREAMER_CAMERA_H
