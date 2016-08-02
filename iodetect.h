//
// Created by shinco on 15-12-24.
//

#ifndef GSTREAMER_IODETECT_H
#define GSTREAMER_IODETECT_H

typedef struct _gpio
{
    char path[100];
    int fd;
    int lowtimes;
    int hightimes;
    char status;
    char oldstatus;
    int count;
} GpioStruct;

gpointer iodetectRun(gpointer);

#endif //GSTREAMER_IODETECT_H
