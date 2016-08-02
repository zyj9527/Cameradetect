//
// Created by shinco on 15-12-24.
//

#include <linux/limits.h>
#include <sys/time.h>
#include <glib.h>
#include "camera.h"
#include "common.h"
#include "touchevent.h"
#include "shellDbus.h"

struct marker_t bg;
struct marker_t logo_car;
struct marker_t track_struct[NUM_OF_PATH];

int fd_fb ;
//struct fb_var_screeninfo vinfo;
//struct fb_fix_screeninfo finfo;
long screen_size;
//char *task_ovbuf;
//unsigned char *task_inbuf;
//char *taskvin_ovbuf;
//unsigned char *taskvin_inbuf;
int buffer_id ;
unsigned long long mask ;
static int old = -1;

int g_input ;
int g_in_width ;
int g_in_height ;
int g_display_width = 1280;
int g_display_height = 720;
int g_display_top = 0;
int g_display_left = 0;
int g_rotate = 0;
int g_vflip = 0;
int g_hflip = 0;
int g_vdi_enable = 0;
int g_vdi_motion = 0;
int g_tb = 0;
int g_frame_size;
int g_output_num_buffers = 4;
int g_capture_num_buffers = 3;
int g_frame_period = 33333;

//int g_fmt = v4l2_fourcc('A', 'B', 'G', 'R');

int g_fmt = V4L2_PIX_FMT_YUYV;

v4l2_std_id g_current_std ;
struct testbuffer capture_buffers[3];
struct testbuffer output_buffers[4];




void setup_track_data(CustomData *);
unsigned int fmt_to_bpp(unsigned int pixelformat);
int load_png(const char *path, struct marker_t *img);
void free_pngs();
void draw(char *,struct  marker_t *img,int,int);
void erase(char *,struct marker_t *img,int,int);
void draw_track_path(int angle);
int start_capturing(int fd);
int v4l_capture_setup(int fd);
int v4l_output_setup(int fd);
int prepare_output(int fd);


static gboolean hasinit = FALSE;

void reset7182(){
    system("\
            i2cset -f -y 0 0x21 0x0f 0x80; \
            sleep 0.1;\
            i2cset -f -y 0 0x21 0x0f 0x00;\
            i2cset -f -y 0 0x21 0x52 0xcd;\
            i2cset -f -y 0 0x21 0x00 0x00;\
            i2cset -f -y 0 0x21 0x0e 0x80;\
            i2cset -f -y 0 0x21 0x9c 0x00;\
            i2cset -f -y 0 0x21 0x9c 0xff;\
            i2cset -f -y 0 0x21 0x0e 0x00;\
            i2cset -f -y 0 0x21 0x0e 0x80;\
            i2cset -f -y 0 0x21 0xd9 0x44;\
            i2cset -f -y 0 0x21 0x0e 0x40;\
            i2cset -f -y 0 0x21 0xe0 0x01;\
            i2cset -f -y 0 0x21 0x0e 0x00;\
            i2cset -f -y 0 0x21 0x17 0x41;\
            i2cset -f -y 0 0x21 0x03 0x0c;\
            i2cset -f -y 0 0x21 0x04 0x07;\
            i2cset -f -y 0 0x21 0x13 0x00;\
            i2cset -f -y 0 0x21 0x1d 0x40;\
            i2cset -f -y 0 0x21 0x0e 0x40;\
            i2cset -f -y 0 0x21 0x84 0x00;\
            i2cset -f -y 0 0x21 0x80 0x80;\
            i2cset -f -y 0 0x21 0x0e 0x00;\
            i2cset -f -y 0 0x21 0x0c 0x20;\
            i2cset -f -y 0 0x21 0x0a 0xc0;\
            ");
    printf("reset7182\n");
}

void Camerainit(CustomData *data)
{
    fd_fb = -1;
    screen_size = 0;
    buffer_id = 0;
    mask = 0x0LL;
    g_input = 1;
    g_capture_num_buffers = 3;
    g_in_width = 0;
    g_in_height = 0;
    g_current_std = V4L2_STD_NTSC;
    data->radarThread = startRadar;
#if 0
    setup_track_data(data);
#endif
    reset7182();
    g_print("Camerainit");
    data->radarThread(data);
}

gpointer startRadar(gpointer arg)
{
    //dma_addr_t outpaddr = -1;
    // size_t  isize = -1, ovsize =-1, isize_vin =-1,ovsize_vin=-1;
    int ret, i, j, fd_capture_v4l, fd_output_v4l;
    struct v4l2_buffer capture_buf, output_buf;
    v4l2_std_id id;
    enum v4l2_buf_type type;

    CustomData *data = (CustomData *) arg;

    //data->isRadarRuning = TRUE;

    if ((fd_capture_v4l = open(V4L_CAPTURE_DEV, O_RDWR, 0)) < 0)
    {
        printf("Unable to open %s\n", V4L_CAPTURE_DEV);
        goto quit;
    }
	if (v4l_capture_setup(fd_capture_v4l) < 0) {
        printf("Setup v4l capture failed.\n");
		close(fd_capture_v4l);
		g_usleep(1000*500);
        goto quit;
    }
#if 0
    if ((fd_output_v4l = open(V4L_OUTPUT_DEV, O_RDWR, 0)) < 0)
    {
        printf("Unable to open %s\n", V4L_OUTPUT_DEV);
		g_usleep(1000*500);
        goto quit;
    }
    if (v4l_output_setup(fd_output_v4l) < 0) {
        printf("Setup v4l output failed.\n");
        close(fd_output_v4l);
		g_usleep(1000*500);
        goto quit;
    }
#endif
	g_print("v4l2 init over!\n");
	
    while (1) {
		if(!data->needRunLoop){
            g_usleep(1000*100);
            continue;
        }
		reset7182();
		g_usleep(1000*100); // make image stable
#if 1
		if ((fd_output_v4l = open(V4L_OUTPUT_DEV, O_RDWR, 0)) < 0) {
        	printf("Unable to open %s\n", V4L_OUTPUT_DEV);
			g_usleep(1000*500);
        	continue;
    	}
    	if (v4l_output_setup(fd_output_v4l) < 0) {
        	printf("Setup v4l output failed.\n");
        	close(fd_output_v4l);
			g_usleep(1000*500);
        	continue;
    	}
#endif
		// Open Framebuffer 
        if ((fd_fb = open(FB_DEV, O_RDWR )) < 0) {
            printf("Unable to open frame buffer %s\n", FB_DEV);
			g_usleep(1000*500);
            continue;
        }
        /* Overlay setting */
        struct mxcfb_gbl_alpha alpha;
        alpha.alpha = 0xff;				//fb1 => alpha=ff, fb0 => alpha=0
        alpha.enable = 1;
        if (ioctl(fd_fb, MXCFB_SET_GBL_ALPHA, &alpha) < 0) {
            printf("Set global alpha failed\n");
			g_usleep(1000*500);
            close(fd_fb);
            continue;
        }
        if (prepare_output(fd_output_v4l) < 0)
        {
            printf("prepare_output failed\n");
			g_usleep(1000*500);
            continue;
        }

        if (start_capturing(fd_capture_v4l) < 0)
        {
            printf("start_capturing failed\n");
			g_usleep(1000*500);
            continue;
        }

        //ioctl(fb, FBIOBLANK, FB_BLANK_UNBLANK);
#if 0
        draw(task_ovbuf,&bg,1024,600);
        draw(task_ovbuf,&logo_car,1024,600);
        task.output.paddr = outpaddr;
        ret = ioctl(fd_ipu, IPU_QUEUE_TASK, &task);
#endif

        printf("start loop @@@@@@@@@@@\r\n");
        struct timeval tv;
        struct timeval tv_start;
        gettimeofday (&tv, NULL);
        g_print("\n*****Start loop time = %ds, %dus *****\n", (unsigned int)tv.tv_sec, (unsigned int)tv.tv_usec);

        for (i = 0; data->needRunLoop; i++)
        {
begin:
            if (!data->needRunLoop) {
                printf("radar program abort\r\n");
                break;
            }
            if (ioctl(fd_capture_v4l, VIDIOC_G_STD, &id)) {
                printf("VIDIOC_G_STD failed.\n");
                break;
            }

            if (id == g_current_std) {
                if (!(id == V4L2_STD_PAL || id == V4L2_STD_NTSC)) {
                    printf("Cannot detect TV standard id=%d\n", id);
                    reset7182();
                    g_usleep(1000*500);
                    goto begin;
                }
            } else {
				if (id == V4L2_STD_PAL || id == V4L2_STD_NTSC) {
                	type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                	ioctl(fd_output_v4l, VIDIOC_STREAMOFF, &type);
                	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                	ioctl(fd_capture_v4l, VIDIOC_STREAMOFF, &type);

                	for (j = 0; j < g_output_num_buffers; j++)
                    	munmap(output_buffers[j].start, output_buffers[j].length);
                	for (j = 0; j < g_capture_num_buffers; j++)
                    	munmap(capture_buffers[j].start, capture_buffers[j].length);
				
                	if (v4l_capture_setup(fd_capture_v4l) < 0) {
                    	printf("Setup v4l capture failed.\n");
                    	break;
                	}
                	if (v4l_output_setup(fd_output_v4l) < 0) {
                    	printf("Setup v4l output failed.\n");
                    	break;	
                	}
                	if (prepare_output(fd_output_v4l) < 0) {
                    	printf("prepare_output failed\n");
                    	break;
                	}
                	if (start_capturing(fd_capture_v4l) < 0) {
                    	printf("start_capturing failed\n");
                    	break;
                	}
                	printf("TV standard changed id=%d\n",g_current_std);
            	} else {
                	printf("Cannot detect TV standard id=%d\n",id);
                	reset7182();
                	sleep(1);
            	}
				goto begin;
			}
next:
            memset(&capture_buf, 0, sizeof(capture_buf));
            capture_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            capture_buf.memory = V4L2_MEMORY_MMAP;
            if (ioctl(fd_capture_v4l, VIDIOC_DQBUF, &capture_buf) < 0) {    
                printf("VIDIOC_DQBUF failed.\n");
                break;
            }
            memset(&output_buf, 0, sizeof(output_buf));
            output_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
            output_buf.memory = V4L2_MEMORY_MMAP;
            if (i < g_output_num_buffers) {
                output_buf.index = i;
                if (ioctl(fd_output_v4l, VIDIOC_QUERYBUF, &output_buf) < 0)
                {
                    printf("VIDIOC_QUERYBUF failed\n");
                    break;
                }
            } else {
                output_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                output_buf.memory = V4L2_MEMORY_MMAP;
                if (ioctl(fd_output_v4l, VIDIOC_DQBUF, &output_buf) < 0)
                {
                    printf("VIDIOC_DQBUF failed\n");
                    break;
                }
            }

            memcpy(output_buffers[output_buf.index].start, capture_buffers[capture_buf.index].start, g_frame_size);
            if (ioctl(fd_capture_v4l, VIDIOC_QBUF, &capture_buf) < 0) {
                printf("VIDIOC_QBUF failed :fd_capture_v4l\n");
                break;
            }

            output_buf.timestamp.tv_sec = tv_start.tv_sec;
            output_buf.timestamp.tv_usec = tv_start.tv_usec + (g_frame_period * i);
            if (g_vdi_enable)
                output_buf.field = g_tb ? V4L2_FIELD_INTERLACED_TB :
                    V4L2_FIELD_INTERLACED_BT;
            if (ioctl(fd_output_v4l, VIDIOC_QBUF, &output_buf) < 0) {
                printf("VIDIOC_QBUF failed :fd_output_v4l\n");
                break;
            }
            if (i == 1) {
                type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                if (ioctl(fd_output_v4l, VIDIOC_STREAMON, &type) < 0) {
                    printf("Could not start stream\n");
                    break;
                }
            }
        }
		printf ("End of loop@@@@@@@@@@\r\n");
		old = -1;
		
		for (i = 0; i < g_output_num_buffers; i++)
        	munmap(output_buffers[i].start, output_buffers[i].length);
    	for (i = 0; i < g_capture_num_buffers; i++)
        	munmap(capture_buffers[i].start, capture_buffers[i].length);
 
		if (fd_output_v4l) {
            type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
            ioctl(fd_output_v4l, VIDIOC_STREAMOFF, &type);
#if 1
			close(fd_output_v4l);
#endif
        }
		if (fd_capture_v4l) {
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            ioctl(fd_capture_v4l, VIDIOC_STREAMOFF, &type);
        }
		if (fd_fb) {
            ioctl(fd_fb, FBIOBLANK, FB_BLANK_NORMAL);
            close(fd_fb);
            fd_fb = -1;
        }
		data->startPlay(data);
    }
quit:
    if (fd_fb) {
        ioctl(fd_fb, FBIOBLANK, FB_BLANK_NORMAL);
        close(fd_fb);
        fd_fb = -1;
    }

    for (i = 0; i < g_output_num_buffers; i++)
        munmap(output_buffers[i].start, output_buffers[i].length);
    for (i = 0; i < g_capture_num_buffers; i++)
        munmap(capture_buffers[i].start, capture_buffers[i].length);

	if (fd_output_v4l) {
        type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        ioctl(fd_output_v4l, VIDIOC_STREAMOFF, &type);
		close(fd_output_v4l);
    }
	if (fd_capture_v4l) {
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(fd_capture_v4l, VIDIOC_STREAMOFF, &type);
		close(fd_capture_v4l);
    }

    printf("radar program end\r\n");
    return 0;
}


void deinit()
{
    free_pngs();
}
void load_bg(CustomData *data){
    bg.startx = START_X_BG;
    bg.starty = START_Y_BG;
    if(bg.buffer != NULL){
        free(bg.buffer);
    }
    if(bg.colormap != NULL){
        free(bg.colormap);
    }
    if (data->language==0x00){
        if (load_png( DATADIR "/bg.png", &bg) < 0)
        {
            printf("Fail to load %s/bg.png\r\n",DATADIR);
        }
    } else if(data->language==0x01){
        if (load_png( DATADIR "/bg_en.png", &bg) < 0)
        {
            printf("Fail to load %s/bg_en.png\r\n",DATADIR);
        }
    }
}
void setup_track_data(CustomData *data)
{

    load_bg(data);
    logo_car.startx = START_X_LOGO_CAR;
    logo_car.starty = START_Y_LOGO_CAR;
    if (load_png( DATADIR"/car-pic.png", &logo_car) < 0)
    {
        printf("Fail to load %s/car-pic.png\r\n",DATADIR);
    }

    char png_path[100];
    int i,j;
    for(i=NUM_OF_PATH/2,j=0;i>0;i--,j++)
    {
        if(i<10){
            snprintf(png_path, PATH_MAX, "%s/left/0%d%s", DATADIR,(i),".png");
        } else{
            snprintf(png_path, PATH_MAX, "%s/left/%d%s", DATADIR,(i),".png");
        }

        track_struct[j].startx = START_X_TRACK;
        track_struct[j].starty = START_Y_TRACK;
        if(load_png(png_path,&track_struct[j])<0)
        {
            printf("Fail to load %s \r\n",png_path);
        }
    }
    snprintf(png_path, PATH_MAX, "%s/%s", DATADIR,"00.png");
    track_struct[j].startx = START_X_TRACK;
    track_struct[j].starty = START_Y_TRACK;
    if(load_png(png_path,&track_struct[j])<0)
    {
        printf("Fail to load %s \r\n",png_path);
    }
    for(i=0,++j;i<NUM_OF_PATH/2;i++,j++)
    {
        if(i+1<10){
            snprintf(png_path, PATH_MAX, "%s/right/0%d%s", DATADIR,(i+1),".png");
        } else{
            snprintf(png_path, PATH_MAX, "%s/right/%d%s", DATADIR,(i+1),".png");
        }
        track_struct[j].startx = START_X_TRACK;
        track_struct[j].starty = START_Y_TRACK;
        if(load_png(png_path,&track_struct[j])<0)
        {
            printf("Fail to load %s \r\n",png_path);
        }
    }
    hasinit = TRUE;

}

unsigned  int fmt_to_bpp(unsigned int pixelformat)
{
    unsigned int bpp;

    switch (pixelformat)
    {
        case IPU_PIX_FMT_RGB565:
            /*interleaved 422*/
        case IPU_PIX_FMT_YUYV:
        case IPU_PIX_FMT_UYVY:
            /*non-interleaved 422*/
        case IPU_PIX_FMT_YUV422P:
        case IPU_PIX_FMT_YVU422P:
            bpp = 16;
            break;
        case IPU_PIX_FMT_BGR24:
        case IPU_PIX_FMT_RGB24:
        case IPU_PIX_FMT_YUV444:
        case IPU_PIX_FMT_YUV444P:
            bpp = 24;
            break;
        case IPU_PIX_FMT_BGR32:
        case IPU_PIX_FMT_BGRA32:
        case IPU_PIX_FMT_RGB32:
        case IPU_PIX_FMT_RGBA32:
        case IPU_PIX_FMT_ABGR32:
            bpp = 32;
            break;
            /*non-interleaved 420*/
        case IPU_PIX_FMT_YUV420P:
        case IPU_PIX_FMT_YVU420P:
        case IPU_PIX_FMT_YUV420P2:
        case IPU_PIX_FMT_NV12:
        case IPU_PIX_FMT_TILED_NV12:
            bpp = 12;
            break;
        default:
            bpp = 8;
            break;
    }
    return bpp;
}

int load_png(const char *path, struct  marker_t *img)
{
    memset(&img->image, 0, sizeof(img->image));
    img->buffer = NULL;
    img->colormap = NULL;
    img->image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_file(&img->image, path) <= 0)
        return -1;

    img->image.format = PNG_FORMAT_ABGR_COLORMAP;

    img->buffer = (unsigned char*)malloc(PNG_IMAGE_SIZE(img->image));

    if (img->buffer == NULL)
        return -1;

    img->colormap = (char *)malloc(PNG_IMAGE_COLORMAP_SIZE(img->image));
    if (img->colormap == NULL){
        free(img->buffer);
        return -1;
    }

    if (png_image_finish_read(&img->image, NULL/*background*/, img->buffer, 0/*row_stride*/, img->colormap/*colormap*/) <= 0)
    {
        if (img->buffer == NULL)
            png_image_free(&img->image);
        else
        {
            free(img->buffer);
            free(img->colormap);
            img->buffer = NULL;
            img->colormap = NULL;
        }
        return -1;
    }
    return 0;
}


void free_pngs()
{
    int i;
    if (bg.buffer)
    {
        free(bg.buffer);
        bg.buffer = NULL;
    }
    if(bg.colormap){
        free(bg.colormap);
        bg.colormap = NULL;
    }
    if(logo_car.colormap){
        free(logo_car.colormap);
        logo_car.colormap = NULL;
    }
    if (logo_car.buffer)
    {
        free(logo_car.buffer);
        logo_car.buffer = NULL;
    }
    for (i = 0; i < NUM_OF_PATH; i++)
    {
        free(track_struct[i].buffer);
        free(track_struct[i].colormap);
        track_struct[i].buffer = NULL;
        track_struct[i].colormap = NULL;
    }
}

void draw(char *buff,struct marker_t *img,int width,int heigth)
{
    int x, y;
    unsigned char *src = NULL;
    char *dest = NULL;
    if (img->buffer == NULL)
    {
        printf("img.buffer is NULL\r\n");
        return;
    }
    src = img->buffer;
    for (y = 0; y < img->image.height; y++)
    {
        if ((img->starty + y) >= heigth)
            break;
        dest = buff + (((img->starty + y) * width + img->startx) * BYTE_PER_PIXEL);
        for (x = 0; x < img->image.width; x++)
        {
            if (img->colormap[*src*BYTE_PER_PIXEL] == 0x00){
                src++;
                dest += BYTE_PER_PIXEL;
            } else if(img->colormap[*src*BYTE_PER_PIXEL] == (unsigned char)0xff){
                *dest++ = img->colormap[*src*BYTE_PER_PIXEL];
                *dest++ = img->colormap[*src*BYTE_PER_PIXEL+1];
                *dest++ = img->colormap[*src*BYTE_PER_PIXEL+2];
                *dest++ = img->colormap[*src*BYTE_PER_PIXEL+3];
                src++;
            }
        }
    }
}

void erase(char *buff,struct  marker_t *img,int width,int heigth)
{
    int x, y;
    char *dest = NULL;
    if (img->buffer == NULL)
    {
        printf("img.buffer is NULL\r\n");
        return;
    }
    unsigned char *mask = img->buffer;
    for (y = 0; y < img->image.height; y++)
    {

        if ((img->starty + y) >= heigth)
        {

            break;
        }
        // src = &bg.buffer[(((track_struct[idx].starty - bg.starty) + y) * bg.image.width + (track_struct[idx].startx - bg.startx)) * BYTE_PER_PIXEL];
        //dest = &pfb[((marker[idx].starty + y) * vinfo.xres + marker[idx].startx) * vinfo.bits_per_pixel / 8];
        dest = buff + (((img->starty + y) * width + img->startx) * BYTE_PER_PIXEL);
        for (x = 0; x < img->image.width; x++)
        {
            if ((img->colormap[*mask*BYTE_PER_PIXEL] == 0x00)/* && (src[x+1] == 0xff) && (src[x+2] == 0xff) && (src[x+3] == 0xff)*/)
            {
                mask ++;
                dest += BYTE_PER_PIXEL;
                continue;
            }
            *dest++ = 0;
            *dest++ = 0;
            *dest++ = 0;
            *dest++ = 0;
            mask ++;
        }
    }
}

int start_capturing(int fd_capture_v4l)
{
    unsigned int i;
    struct v4l2_buffer buf;
    enum v4l2_buf_type type;

    for (i = 0; i < g_capture_num_buffers; i++)     
    {
        memset(&buf, 0, sizeof (buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd_capture_v4l, VIDIOC_QUERYBUF, &buf) < 0)   
        {
            printf("VIDIOC_QUERYBUF error\n");
            return TFAIL;
        }

        capture_buffers[i].length = buf.length;
        printf("capture_buffers[%d].length: %u\n", i, capture_buffers[i].length);
        capture_buffers[i].offset = (size_t) buf.m.offset;
        capture_buffers[i].start = (unsigned char *)mmap (NULL, capture_buffers[i].length,
                PROT_READ | PROT_WRITE, MAP_SHARED,
                fd_capture_v4l, capture_buffers[i].offset);
        memset(capture_buffers[i].start, 0xFF, capture_buffers[i].length);
    }

    for (i = 0; i < g_capture_num_buffers; i++)
    {
        memset(&buf, 0, sizeof (buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        buf.m.offset = capture_buffers[i].offset;
        if (ioctl (fd_capture_v4l, VIDIOC_QBUF, &buf) < 0) {  
            printf("VIDIOC_QBUF error\n");
            return TFAIL;
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl (fd_capture_v4l, VIDIOC_STREAMON, &type) < 0) {
        printf("VIDIOC_STREAMON error\n");
        return TFAIL;
    }
    return 0;
}

int v4l_capture_setup(int fd_capture_v4l)
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_dbg_chip_ident chip;
    struct v4l2_streamparm parm;
    v4l2_std_id id;
    unsigned int min;

    if (ioctl (fd_capture_v4l, VIDIOC_QUERYCAP, &cap) < 0) {    //查询驱动功能
        if (EINVAL == errno) {
            fprintf (stderr, "%s is no V4L2 device\n",
                    V4L_CAPTURE_DEV);
            return TFAIL;
        } else {
            fprintf (stderr, "%s isn not V4L device,unknow error\n",
                    V4L_CAPTURE_DEV);
            return TFAIL;
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf (stderr, "%s is no video capture device\n",
                V4L_CAPTURE_DEV);
        return TFAIL;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf (stderr, "%s does not support streaming i/o\n",
                V4L_CAPTURE_DEV);
        return TFAIL;
    }

    if (ioctl(fd_capture_v4l, VIDIOC_DBG_G_CHIP_IDENT, &chip))  //识别芯片
    {
        printf("VIDIOC_DBG_G_CHIP_IDENT failed.\n");
        close(fd_capture_v4l);
        return TFAIL;
    }

    printf("TV decoder chip is %s,reversion is %d\n", chip.match.name,chip.revision);

    if (ioctl(fd_capture_v4l, VIDIOC_S_INPUT, &g_input) < 0)    //视频输入 1
    {
        printf("VIDIOC_S_INPUT failed\n");
        close(fd_capture_v4l);
        return TFAIL;
    }

    if (ioctl(fd_capture_v4l, VIDIOC_G_STD, &id) < 0)
    {
        printf("VIDIOC_G_STD failed\n");
        close(fd_capture_v4l);
        return TFAIL;
    }

    g_current_std = id;

    if (ioctl(fd_capture_v4l, VIDIOC_S_STD, &id) < 0)
    {
        printf("VIDIOC_S_STD failed\n");
        close(fd_capture_v4l);
        return TFAIL;
    }

    /* Select video input, video standard and tune here. */

    memset(&cropcap, 0, sizeof(cropcap));

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl (fd_capture_v4l, VIDIOC_CROPCAP, &cropcap) < 0) {
        memset(&crop, 0, sizeof(crop));
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */
        fprintf(stdout,"ioctl (fd_capture_v4l, VIDIOC_CROPCAP, &cropcap) < 0");

        if (ioctl (fd_capture_v4l, VIDIOC_S_CROP, &crop) < 0) {
            switch (errno) {
                case EINVAL:
                    /* Cropping not supported. */
                    fprintf (stderr, "%s  doesn't support crop\n",
                            V4L_CAPTURE_DEV);
                    break;
                default:
                    /* Errors ignored. */
                    break;
            }
        }
    } else {
        /* Errors ignored. */

    }

    fprintf(stdout,"cropcap:l=%d,t=%d,w=%d,h=%d\n",cropcap.defrect.left,cropcap.defrect.top,cropcap.defrect.width,cropcap.defrect.height);


    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.timeperframe.numerator = 1;
    parm.parm.capture.timeperframe.denominator = 0;
    parm.parm.capture.capturemode = 0;
    if (ioctl(fd_capture_v4l, VIDIOC_S_PARM, &parm) < 0)
    {
        printf("VIDIOC_S_PARM failed\n");
        close(fd_capture_v4l);
        return TFAIL;
    }

    memset(&fmt, 0, sizeof(fmt));

    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = 0;
    fmt.fmt.pix.height      = 0;
    fmt.fmt.pix.pixelformat = g_fmt;//V4L2_PIX_FMT_UYVY;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

    if (ioctl (fd_capture_v4l, VIDIOC_S_FMT, &fmt) < 0){
        fprintf (stderr, "%s iformat not supported \n",
                V4L_CAPTURE_DEV);
        return TFAIL;
    }

    /* Note VIDIOC_S_FMT may change width and height. */
    fprintf(stdout,"window.l=%d,window.t=%d,window.w=%d,window.h=%d\n",fmt.fmt.win.w.left,fmt.fmt.win.w.top,fmt.fmt.win.w.width,fmt.fmt.win.w.height);
    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;

    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;

    if (ioctl(fd_capture_v4l, VIDIOC_G_FMT, &fmt) < 0)
    {
        printf("VIDIOC_G_FMT failed\n");
        close(fd_capture_v4l);
        return TFAIL;
    }

    g_in_width = fmt.fmt.pix.width;
    g_in_height = fmt.fmt.pix.height;
    printf("fmt.fmt.pix.width: %d, fmt.fmt.pix.height: %d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
    printf("fmt.fmt.pix.bytesperline: %d, fmt.fmt.pix.sizeimage: %d\n", fmt.fmt.pix.bytesperline, fmt.fmt.pix.sizeimage);
    printf("VIDIOC_G_FMT OK\n");

    memset(&req, 0, sizeof (req));

    req.count               = g_capture_num_buffers;
    req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory              = V4L2_MEMORY_MMAP;

    if (ioctl (fd_capture_v4l, VIDIOC_REQBUFS, &req) < 0) {
        if (EINVAL == errno) {
            fprintf (stderr, "%s does not support "
                    "memory mapping\n", V4L_CAPTURE_DEV);
            return TFAIL;
        } else {
            fprintf (stderr, "%s does not support "
                    "memory mapping, unknow error\n", V4L_CAPTURE_DEV);
            return TFAIL;
        }
    }

    if (req.count < 2) {
        fprintf (stderr, "Insufficient buffer memory on %s\n",
                V4L_CAPTURE_DEV);
        return TFAIL;
    }

    return 0;
}

int v4l_output_setup(int fd_output_v4l)
{
    struct v4l2_control ctrl;
    struct v4l2_format fmt;
    struct v4l2_framebuffer fb;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_capability cap;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_requestbuffers buf_req;

    if (!ioctl(fd_output_v4l, VIDIOC_QUERYCAP, &cap)) {
        printf("driver=%s, card=%s, bus=%s, "
                "version=0x%08x, "
                "capabilities=0x%08x\n",
                cap.driver, cap.card, cap.bus_info,
                cap.version,
                cap.capabilities);
    }

    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    while (!ioctl(fd_output_v4l, VIDIOC_ENUM_FMT, &fmtdesc)) {
        printf("fmt %s: fourcc = 0x%08x\n",
                fmtdesc.description,
                fmtdesc.pixelformat);
        fmtdesc.index++;
    }

    memset(&cropcap, 0, sizeof(cropcap));
    cropcap.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    if (ioctl(fd_output_v4l, VIDIOC_CROPCAP, &cropcap) < 0)
    {
        printf("get crop capability failed\n");
        close(fd_output_v4l);
        return TFAIL;
    }

    crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    crop.c.top = g_display_top;
    crop.c.left = g_display_left;
    crop.c.width = g_display_width;
    crop.c.height = g_display_height;
    if (ioctl(fd_output_v4l, VIDIOC_S_CROP, &crop) < 0)
    {
        printf("set crop failed\n");
        close(fd_output_v4l);
        return TFAIL;
    }

    // Set rotation
    ctrl.id = V4L2_CID_ROTATE;
    ctrl.value = g_rotate;
    if (ioctl(fd_output_v4l, VIDIOC_S_CTRL, &ctrl) < 0)
    {
        printf("set ctrl rotate failed\n");
        close(fd_output_v4l);
        return TFAIL;
    }
    ctrl.id = V4L2_CID_VFLIP;
    ctrl.value = g_vflip;
    if (ioctl(fd_output_v4l, VIDIOC_S_CTRL, &ctrl) < 0)
    {
        printf("set ctrl vflip failed\n");
        close(fd_output_v4l);
        return TFAIL;
    }
    ctrl.id = V4L2_CID_HFLIP;
    ctrl.value = g_hflip;
    if (ioctl(fd_output_v4l, VIDIOC_S_CTRL, &ctrl) < 0)
    {
        printf("set ctrl hflip failed\n");
        close(fd_output_v4l);
        return TFAIL;
    }
    if (g_vdi_enable) {
        ctrl.id = V4L2_CID_MXC_MOTION;
        ctrl.value = g_vdi_motion;
        if (ioctl(fd_output_v4l, VIDIOC_S_CTRL, &ctrl) < 0)
        {
            printf("set ctrl motion failed\n");
            close(fd_output_v4l);
            return TFAIL;
        }
    }

    fb.flags = V4L2_FBUF_FLAG_OVERLAY;
    ioctl(fd_output_v4l, VIDIOC_S_FBUF, &fb);

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    fmt.fmt.pix.width= g_in_width;
    fmt.fmt.pix.height= g_in_height - 2; // delete bad border
    fmt.fmt.pix.pixelformat = g_fmt;
    fmt.fmt.pix.bytesperline = g_in_width;
    fmt.fmt.pix.priv = 0;
    fmt.fmt.pix.sizeimage = 0;
    if (g_tb)
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED_TB;
    else
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED_BT;
    if (ioctl(fd_output_v4l, VIDIOC_S_FMT, &fmt) < 0)
    {
        printf("set format failed\n");
        return TFAIL;
    }

    if (ioctl(fd_output_v4l, VIDIOC_G_FMT, &fmt) < 0)
    {
        printf("get format failed\n");
        return TFAIL;
    }
    g_frame_size = fmt.fmt.pix.sizeimage;

    memset(&buf_req, 0, sizeof(buf_req));
    buf_req.count = g_output_num_buffers;
    buf_req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    buf_req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd_output_v4l, VIDIOC_REQBUFS, &buf_req) < 0)
    {
        printf("request buffers failed\n");
        return TFAIL;
    }

    return 0;
}

int prepare_output(int fd_output_v4l)
{
    int i;
    struct v4l2_buffer output_buf;

    for (i = 0; i < g_output_num_buffers; i++)
    {
        memset(&output_buf, 0, sizeof(output_buf));
        output_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        output_buf.memory = V4L2_MEMORY_MMAP;
        output_buf.index = i;
        if (ioctl(fd_output_v4l, VIDIOC_QUERYBUF, &output_buf) < 0)
        {
            printf("VIDIOC_QUERYBUF error\n");
            return TFAIL;
        }

        output_buffers[i].length = output_buf.length;
        output_buffers[i].offset = (size_t) output_buf.m.offset;
        output_buffers[i].start = mmap (NULL, output_buffers[i].length,
                PROT_READ | PROT_WRITE, MAP_SHARED,
                fd_output_v4l, output_buffers[i].offset);
        if (output_buffers[i].start == NULL) {
            printf("v4l2 tvin test: output mmap failed\n");
            return TFAIL;
        }
    }
    return 0;
}

#if 0
void draw_track_path(int angle){
    //    g_print("draw_track_path:angle=%d\n",angle);
    int maxdeg = 550;
    int mindeg = -550;
    if(angle>maxdeg){
        angle = maxdeg;
    } else if(angle <mindeg)
    {
        angle = mindeg;
    }
    int degforpic = (maxdeg-mindeg)/NUM_OF_PATH;
    int i = NUM_OF_PATH/2+(-angle/degforpic);
    if(i!=old)
    {   if(0<=old&&old<NUM_OF_PATH){
                                       erase(taskvin_ovbuf,&track_struct[old],camera_width,camera_height);
                                   }
    draw(taskvin_ovbuf,&track_struct[i],camera_width,camera_height);
    old = i;
    }

}
#endif
