//
// Created by shinco on 15-12-24.
//

#include <glib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "touchevent.h"
#include "common.h"
int fd;
gboolean stopflag;
input_slot input_slots[5];

void TouchEventStop()
{
    stopflag = TRUE;
}
gpointer TouchEventRun(gpointer arg) {
    struct input_event event;
    int currentslot = 0;
    CustomData *data = (CustomData *)arg;
    memset(input_slots, 0, sizeof(input_slots));
    stopflag = FALSE;
    fd = open(INPUT_DEV,O_RDONLY);
    if(fd<0)
    {
        g_print("open touchevent failed");
        return NULL;
    }

    g_print("TouchEventStart\n");
    while (!stopflag) {
        if (read(fd, &event, sizeof(struct input_event)) == sizeof(struct input_event)) {
            switch (event.type) {
                case EV_ABS:
                    switch (event.code) {
                        case ABS_MT_SLOT:
                            currentslot = event.value;
                            break;
                        case ABS_MT_TRACKING_ID:
                            input_slots[currentslot].tracking_id = event.value;
                            break;
                        case ABS_MT_POSITION_X:

                            input_slots[currentslot].x = event.value;
                            break;
                        case ABS_MT_POSITION_Y:

                            input_slots[currentslot].y = event.value;
                            break;
                        default:
                            break;
                    }
                    break;
                case EV_SYN:
                    if (event.code == SYN_REPORT) {
                        if (currentslot == 0 && input_slots[currentslot].tracking_id == -1) {

                            g_print("input:x=%d,y=%d\n",input_slots[currentslot].x,input_slots[currentslot].y);
                            if(data->serialPortInterface!=NULL){
                                GVariantBuilder *builder;
                                GVariant *value;
                                builder = g_variant_builder_new (G_VARIANT_TYPE ("ay"));

                                g_variant_builder_add(builder,"y",input_slots[currentslot].x&0xff);
                                g_variant_builder_add(builder,"y",input_slots[currentslot].x>>8&0xff);
                                g_variant_builder_add(builder,"y",input_slots[currentslot].y&0xff);
                                g_variant_builder_add(builder,"y",input_slots[currentslot].y>>8&0xff);
                                g_variant_builder_add(builder,"y",0x00);
                                value = g_variant_new ("ay", builder);
                                g_variant_builder_unref (builder);
                                serial_port_interface_call_send_data_to_audio(data->serialPortInterface,TRUE,0x5c,value,NULL,NULL,NULL);

                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }else{
            g_print("read input event err\n");
        }
        g_usleep(10*1000);
    }
    close(fd);
    g_print("TouchEventStop\n");
}

void TouchEventStart(CustomData *data) {
    stopflag = FALSE;
    g_thread_new("touchEventThread",TouchEventRun,data);

}
