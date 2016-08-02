//
// Created by shinco on 15-12-24.
//

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib.h>
#include "common.h"
#include "iodetect.h"


GpioStruct gpio_84={
        .path="/sys/class/gpio/gpio84/value",
        .fd=-1,
        .count=0,
        .hightimes=0,
        .lowtimes=10,
        .status=-1,
        .oldstatus=-1
};
GpioStruct gpio_85={
        .path="/sys/class/gpio/gpio85/value",
        .fd=-1,
        .count=0,
        .hightimes=20,
        .lowtimes=100,
        .status=-1,
        .oldstatus=-1
};
GpioStruct gpio_86={
        .path="/sys/class/gpio/gpio86/value",
        .fd=-1,
        .count=0,
        .hightimes=20,
        .lowtimes=100,
        .status=-1,
        .oldstatus=-1
};

int openGpio(GpioStruct *gpio) {

    if((gpio->fd = open(gpio->path,O_RDONLY))<0)
    {
        g_print("open:%s failed\n",gpio->path);
        return -1;
    }
    return 0;
}


gpointer iodetectRun(gpointer arg)
{
    CustomData *data = (CustomData *) arg;
    openGpio(&gpio_84);
    openGpio(&gpio_85);
    openGpio(&gpio_86);
    if(gpio_84.fd < 0 || gpio_85.fd < 0 || gpio_86.fd < 0)
    {
        g_print("openGpio err\n");
        return NULL;
    }

//    system("rm -rf /var/radar");
    while(1) {
        if(lseek(gpio_84.fd,0,SEEK_SET)==-1) {
            g_print("lseek err\n");
            g_usleep(10*1000);
            break;
        }

        if(read(gpio_84.fd,&gpio_84.status,1)<1) {
            g_print("read err\n");
            g_usleep(10*1000);
            break;
        }else {
            if(gpio_84.status != gpio_84.oldstatus) {
                if(gpio_84.status=='0') {
                    if(gpio_84.count ++ > gpio_84.lowtimes) {
                        gpio_84.oldstatus = gpio_84.status;
                       	gpio_84.count = 0;
                        data->isReverse=gpio_84.status;
						if (!data->needRunLoop) {
                                g_print("startPlay\n");
                                data->startPlay(data);
                            } else {
                                g_print("radar is  running\n");
                            }
                            g_print("gpio_84.status:%c\n", gpio_84.status);
                            data->needRunLoop=(data->isReverse=='1')||(data->isManualReverse);
                    }
                }else if(gpio_84.status=='1') {
                    if(gpio_84.count ++ > gpio_84.hightimes) {
                        gpio_84.oldstatus = gpio_84.status;
                        gpio_84.count = 0;
                        data->isReverse=gpio_84.status;
                        data->stopPlay(data);
                        g_print("gpio_84.status:%c\n",gpio_84.status);
                        //system("echo 1 > /var/radar");
                	}
				}

            }else{
                gpio_84.count = 0;
            }

        }


        //////////////////////////////////////////////////////////
        if(lseek(gpio_85.fd,0,SEEK_SET)==-1){
            g_print("lseek err\n");
            g_usleep(10*1000);
            break;
        }

        if(read(gpio_85.fd,&gpio_85.status,1)<1){
            g_print("read err\n");
            g_usleep(10*1000);
            break;
        }else {
            if(gpio_85.status != gpio_85.oldstatus) {
                if(gpio_85.status=='0') {
                    if(gpio_85.count ++ > gpio_85.lowtimes) {
                        gpio_85.oldstatus = gpio_85.status;
                        gpio_85.count = 0;
                        system("echo '1' > /sys/class/gpio/gpio56/value");

                        g_print("gpio_85.status:%c\n",gpio_85.status);
                    }
                }else if(gpio_85.status=='1') {
                    if(gpio_85.count ++ > gpio_85.hightimes) {
                        gpio_85.oldstatus = gpio_85.status;
                        gpio_85.count = 0;
                        system("echo '0' > /sys/class/gpio/gpio56/value");

                        g_print("gpio_85.status:%c\n",gpio_85.status);
                    }
                }
            }else{
                gpio_85.count = 0;
            }
        }

        ////////////////////////////////////////////////////////////////////
        if(lseek(gpio_86.fd,0,SEEK_SET)==-1) {
            g_usleep(10*1000);
            break;
        }

        if(read(gpio_86.fd,&gpio_86.status,1)<1) {
            g_usleep(10*1000);
            break;
        }else {
            if(gpio_86.status != gpio_86.oldstatus) {
                if(gpio_86.status=='0') {
                    if(gpio_86.count ++ > gpio_86.lowtimes) {
                        gpio_86.oldstatus = gpio_86.status;
                        gpio_86.count = 0;
                        system("echo '1' > /sys/class/gpio/gpio87/value");
                        g_print("gpio_86.status:%c\n",gpio_86.status);
                    }
                }else if(gpio_86.status=='1') {
                    if(gpio_86.count ++ > gpio_86.hightimes) {
                        gpio_86.oldstatus = gpio_86.status;
                        gpio_86.count = 0;
                        system("echo '0' > /sys/class/gpio/gpio87/value");
                        g_print("gpio_86.status:%c\n",gpio_86.status);
                    }
                }
            }else{
                gpio_86.count = 0;
            }
        }

        g_usleep(50*1000);
    }
    g_print("iodetectRun exit\n");
    g_thread_exit(NULL);
}





