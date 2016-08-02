//
// Created by shinco on 15-12-24.
//

#ifndef GSTREAMER_TOUCHEVENT_H
#define GSTREAMER_TOUCHEVENT_H

#include <linux/input.h>
#include "common.h"

#define INPUT_DEV       "/dev/input/touchscreen0"
typedef struct _slot
{
    int tracking_id;
    int lastx;
    int lasty;
    int x;
    int y;
}input_slot;

void TouchEventStop();
void TouchEventStart(CustomData *data);

#endif //GSTREAMER_TOUCHEVENT_H
