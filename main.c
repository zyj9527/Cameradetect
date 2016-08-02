#include "common.h"
#include "iodetect.h"
#include "camera.h"


CustomData customData;
static gboolean hasinit = FALSE;
#if 0
static gboolean timeoutCallback(gpointer arg)
{
    CustomData *data = (CustomData *) arg;
	data->duration++;
	if(data->isplaydone)
        return FALSE;
	
    if (data->playbin && data->duration > 700) {
		g_print ("End of stream\n");
		data->isplaydone=TRUE;
		if(data->playbin!=NULL) {
    	gst_element_set_state(data->playbin, GST_STATE_NULL);
    	gst_object_unref(data->playbin);
    	data->playbin = NULL;
    	}	
	}	
    return TRUE;
}

static gboolean weakupTimeoutCallback(gpointer arg)
{
    CustomData *data = (CustomData *) arg;
   if(hasinit){
       return FALSE;
   }else{
       GVariantBuilder *builder;
       GVariant *value;
       builder = g_variant_builder_new (G_VARIANT_TYPE ("ay"));
       value = g_variant_new ("ay", builder);
       g_variant_builder_unref (builder);
	   serial_port_interface_call_send_data_to_audio(customData.serialPortInterface,TRUE,0xFC,value,NULL,NULL,NULL);
       //serial_port_interface_call_send_data_to_audio(customData.serialPortInterface,TRUE,0x06,value,NULL,NULL,NULL);
       return TRUE;
   }
}
#endif

void stopPlay(gpointer arg)
{
    CustomData *data = (CustomData *) arg;
	if(data->needRunLoop)
        return;
    if(data->isReverse=='1'|| data->isManualReverse)
    {
		#if 1
    	if (data->carplayConnected && data->carplayPlaying) {  //Carplay Connected
        	g_print("\nDetect radar: Carplay connected and playing!\n");
			if ( ! data->carplayBorrowed ) {
				GVariantBuilder *builder;
       			GVariant *value;
       			builder = g_variant_builder_new (G_VARIANT_TYPE ("a{sv}"));
       			value = g_variant_new ("a{sv}", builder);
       			g_variant_builder_unref (builder);
				if (shn_interface_call_send_to_dbus_sync(data->shellInterface,IPC_CAMERA_ID,WM_CARPLAY_CTRL_BORROW_SCREEN_NEVER,0,value,NULL,NULL)) {
					g_print ("\n**********Call send_to_dbus_sync Success**********\n");
					data->carplayBorrowed = TRUE;
				}else {
					g_print ("\n**********Call send_to_dbus_sync Failed**********\n");
				}
			}
		} else {
			data->needRunLoop=(data->isReverse=='1')||(data->isManualReverse);
		}
		#endif
		
    } 
}

#if 1
void startPlay(gpointer arg)
{
    CustomData *data = (CustomData *) arg;
	#if 1
	if (data->carplayBorrowed) {
		GVariantBuilder *builder;
    	GVariant *value;
    	builder = g_variant_builder_new (G_VARIANT_TYPE ("a{sv}"));
    	value = g_variant_new ("a{sv}", builder);
    	g_variant_builder_unref (builder);
		if (shn_interface_call_send_to_dbus_sync(data->shellInterface,IPC_CAMERA_ID,WM_CARPLAY_CTRL_UNBORROW_SCREEN,0,value,NULL,NULL)) {
			g_print ("\n**********Call send_to_dbus_sync Success**********\n");	
		}else {
			g_print ("\n**********Call send_to_dbus_sync Failed**********\n");
		}
		data->carplayBorrowed = FALSE;
	}
	#endif
}
#endif

static gboolean bus_call (GstBus *bus,GstMessage *msg, gpointer orgdata)
{
        CustomData *data = (CustomData *) orgdata;
        GMainLoop *loop = data->loop;
        switch (GST_MESSAGE_TYPE (msg)) {
            case GST_MESSAGE_EOS:
#if 0
                g_print ("End of stream\n");
                data->isplaydone=TRUE;
                if(data->playbin!=NULL) {
                    gst_element_set_state(data->playbin, GST_STATE_NULL);
                    gst_object_unref(data->playbin);
                    data->playbin = NULL;
                }
                //g_main_loop_quit (loop);
                system("/usr/bin/pulseaudio --kill -v --log-level=4");
#endif
                break;
            case GST_MESSAGE_ERROR: {
                gchar *debug;
                GError *error;
                gst_message_parse_error (msg, &error, &debug);
                g_free (debug);
                g_printerr ("Error: %s\n", error->message);
                g_error_free (error);
                //g_main_loop_quit (loop);
                break;
            }
#if 0
            case GST_MESSAGE_STATE_CHANGED: {
                GstState old_state, new_state, pending_state;
                gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
                if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->playbin)) {
                    g_print("Pipeline state changed from %s to %s:\n",
                            gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));

                    /* Remember whether we are in the PLAYING state or not */
                    data->playing = (new_state == GST_STATE_PLAYING);
                }
                break;
            }
#endif
            default:
                break;
        }
        return TRUE;
}

#if 0
void getDataFromAudio(SerialPortInterface *serialPortInterface,guchar arg_cmd,
                      const GVariant *arg_data)
{

    if(arg_cmd==0x58)
    {
        GBytes *data = g_variant_get_data_as_bytes(arg_data);
        GByteArray *arry = g_bytes_unref_to_array(data);
        if(arry->len>1)
        {
            customData.angle = (arry->data[0]<<8|arry->data[1])/16-2048;
            g_print("getDataFromAudio:angle=%d\n",customData.angle);
        }
        g_byte_array_unref(arry);
    } else if(hasinit == FALSE && arg_cmd==0xFC){
#if 0
        GVariantBuilder *builder;
        GVariant *value;
        builder = g_variant_builder_new (G_VARIANT_TYPE ("ay"));
        value = g_variant_new ("ay", builder);
        g_variant_builder_unref (builder);
        serial_port_interface_call_send_data_to_audio(customData.serialPortInterface,TRUE,0x12,value,NULL,NULL,NULL);
#endif
        hasinit = TRUE;
        GBytes *data = g_variant_get_data_as_bytes(arg_data);
        GByteArray *arry = g_bytes_unref_to_array(data);
#if 0
        if(arry->len>=22)
        {
            customData.is360 = (int)(arry->data[20]&0x01)==0?FALSE:TRUE;
            customData.hasVideoAssert = (int)(arry->data[21]&0x10)==0?FALSE:TRUE;
            customData.language = arry->data[20]>>4&0x0f;
            g_print("getDataFromAudio:is360=%d,hasVideoAssert=%d,language=%x\n",customData.is360,customData.hasVideoAssert,customData.language);
        }
#endif
#if 0
		if (arry->data[7] == 0x41) {
			g_print ("\n**********This is Normal model**********\n");
			g_thread_new("camerainit",Camerainit,&customData);
        	GThread *ioThread = g_thread_new("iodetect",iodetectRun,&customData);
			g_timeout_add(TIMEOUT,timeoutCallback,&customData);
		}
		else if (arry->data[7] == 0x42){
			g_print ("\n**********This is MCU update model**********\n");
			customData.isplaydone=TRUE;
			if(customData.playbin!=NULL) {
    			gst_element_set_state(customData.playbin, GST_STATE_NULL);
    			gst_object_unref(customData.playbin);
    			customData.playbin = NULL;
    		}	
		}
		else
			g_print ("\n**********MCU Version Error**********\n");
        g_byte_array_unref(arry);
#endif
#if 0
        g_thread_new("camerainit",Camerainit,&customData);
//          Camerainit(&customData);
        GThread *ioThread = g_thread_new("iodetect",iodetectRun,&customData);

        g_timeout_add(TIMEOUT,timeoutCallback,&customData);
#endif
    }else if(arg_cmd==0x5e){
        GBytes *data = g_variant_get_data_as_bytes(arg_data);
        GByteArray *arry = g_bytes_unref_to_array(data);
        if(arry->len>0)
        {
            if(arry->data[0]&0x01==0x01){
                customData.isManualReverse = TRUE;
 //               if(customData.isRadarRuning==FALSE){
                    customData.stopPlay(&customData);
                }

            }else{
                customData.isManualReverse = FALSE;
            }
            g_print("360 display:%d\n",customData.isManualReverse);
        }
    } else if (arg_cmd==0x16){
        GBytes *data = g_variant_get_data_as_bytes(arg_data);
        GByteArray *arry = g_bytes_unref_to_array(data);
        if(arry->len>=22)
        {
            customData.is360 = (int)(arry->data[20]&0x01)==0?FALSE:TRUE;
            customData.hasVideoAssert = (int)(arry->data[21]&0x10)==0?FALSE:TRUE;
            if(customData.language != arry->data[20]>>4&0x0f){
                customData.language = arry->data[20]>>4&0x0f;
                if(customData.language==0x00 || customData.language==0x01){
                    load_bg(&customData);
                }
            }
            g_print("getDataFromAudio:is360=%d,hasVideoAssert=%d,language=%x\n",customData.is360,customData.hasVideoAssert,customData.language);

        }
        g_byte_array_unref(arry);
    }

}
#endif
#if 0
void reverseMessage(ShnInterface *shellinterface,gint type,gint wpram ,gint lpram)
{
    g_print("camera reverseMessage:%d,%d,%d\n",type,wpram,lpram);
    if(customData.is360==FALSE){
        if(type==0&&wpram==0){
            if(lpram==1)
            {
                customData.isManualReverse = TRUE;
                if(customData.isRadarRuning==FALSE){
                    customData.stopPlay(&customData);
                }
            }else{
                customData.isManualReverse = FALSE;
            }
        }
    }

}
#endif

void reverseMessage(ShnInterface *shellinterface,gint type,gint wpram ,gint lpram)
{
	g_print("Carplay Message:%d,%d,%d\n",type,wpram,lpram);
	switch (type) {
		case WM_CARPLAY_CONNECTED:
			customData.carplayConnected = TRUE;
			customData.carplayPlaying= TRUE;
			g_print ("\n------Get Message: WM_CARPLAY_CONNECTED------\n");
			break;
		case WM_CARPLAY_DISCONNECTED:
			customData.carplayConnected = FALSE;
			customData.carplayPlaying= FALSE;
			g_print ("\n------Get Message: WM_CARPLAY_DISCONNECTED------\n");
			break;
		case WM_CARPLAY_CTRL_BORROWACK:
			customData.carplayPlaying= FALSE; 
			g_print ("\n------Get Message: WM_CARPLAY_CTRL_BORROWACK------\n");
			if(customData.isReverse=='1')
                    printf("Carplay is hiden,now reverse\n");
                customData.needRunLoop=(customData.isReverse=='1')||(customData.isManualReverse);
			break;
		case WM_CARPLAY_CTRL_UNBORROWACK:
			customData.carplayPlaying= TRUE;
			g_print ("\n------Get Message: WM_CARPLAY_CTRL_UNBORROWACK------\n");
			break;
		default:
			g_print("Error: Unknown reverseMessage!\n");
			break;
	}
}

#if 0
void Message(ShnInterface *shellinterface,gint arg_from,gint type,gint wpram ,GVariant *arg_lpram)
{
	g_print("Carplay Message:%d,%d,%d\n",arg_from,type,wpram);
	switch (type) {
		case WM_CARPLAY_CONNECTED:
			customData.carplayConnected = TRUE;
			customData.carplayPlaying= TRUE;
			g_print ("\n------Get Message: WM_CARPLAY_CONNECTED------\n");
			break;
		case WM_CARPLAY_DISCONNECTED:
			customData.carplayConnected = FALSE;
			customData.carplayPlaying= FALSE;
			g_print ("\n------Get Message: WM_CARPLAY_DISCONNECTED------\n");
			break;
		case WM_CARPLAY_CTRL_BORROWACK:
			customData.carplayPlaying= FALSE;
			g_print ("\n------Get Message: WM_CARPLAY_CTRL_BORROWACK------\n");
			break;
		case WM_CARPLAY_CTRL_UNBORROWACK:
			customData.carplayPlaying= TRUE;
			g_print ("\n------Get Message: WM_CARPLAY_CTRL_UNBORROWACK------\n");
			break;
		default:
			g_print("Error: Unknown arg_from!\n");
			break;
	}
}
#endif


int  main(int argc, char *argv[]) {

    GstBus *bus;
    GMainLoop *loop;
    GError *error=NULL;
    guint bus_watch_id;

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

#if 0
    /* Create the elements */
    customData.playbin = gst_element_factory_make ("playbin2", "play");

    if (!customData.playbin) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }
#endif

    loop = g_main_loop_new (NULL, FALSE);

    customData.playing = FALSE;
    customData.isplaydone = FALSE;
    customData.loop = loop;
    customData.isReverse = -1;
    customData.serialPortInterface = NULL;
    customData.isManualReverse = FALSE;
    customData.angle = 0;
    customData.shellInterface= NULL;
 
	customData.carplayConnected = FALSE;
	customData.carplayPlaying = FALSE;
	customData.carplayBorrowed = FALSE;
	customData.needRunLoop=FALSE;

    customData.startPlay =startPlay;
    customData.stopPlay = stopPlay;

    /* Set the URI to play */
    //g_object_set (customData.playbin, "uri", "file:///boot/bootanimation.mp4", NULL);
#if 0
	/* Set Element*/
	customData.playbin = gst_pipeline_new ("png-player");
	customData.source = gst_element_factory_make ("filesrc", "file-source");
	customData.decoder = gst_element_factory_make ("pngdec", "png-decoder");
	//customData.freeze = gst_element_factory_make ("freeze", "png-freeze");
	customData.sink= gst_element_factory_make ("autovideosink", "png-output");
	if (!customData.playbin || !customData.source || !customData.decoder || !customData.sink) {
	  		g_print ("One element could not be created\n");
	  		return -1;
	}
	
	g_object_set (G_OBJECT(customData.source), "location", "/home/root/shell/logo.png", NULL);

    /* Listen to the bus */
    bus = gst_pipeline_get_bus (customData.playbin);
    bus_watch_id = gst_bus_add_watch (bus, bus_call, &customData);

	// Add bin and Link element
	gst_bin_add_many (GST_BIN (customData.playbin),customData.source, customData.decoder, customData.sink, NULL);
	gst_element_link_many (customData.source, customData.decoder, customData.sink, NULL);
	
    gst_element_set_state(customData.playbin,GST_STATE_PAUSED);
#endif
    customData.serialPortInterface = serial_port_interface_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,G_DBUS_PROXY_FLAGS_NONE,"com.shinco.shincoservice","/serialport",NULL,&error);
    if(!customData.serialPortInterface) {
        g_print("can not serial_port_interface_proxy_new_for_bus_sync:%s",error->message);
        g_error_free(error);
    }else{
        //g_signal_connect(customData.serialPortInterface,"get-data-from-audio",G_CALLBACK(getDataFromAudio),NULL);
        /*weak up*/
        //g_timeout_add(500,weakupTimeoutCallback,&customData);
    }
    customData.shellInterface = shn_interface_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,G_DBUS_PROXY_FLAGS_NONE,"com.shn.ipc","/ipc/audio",NULL,&error);
	
    if(!customData.shellInterface) {
        g_print("can not audio_interface_proxy_new_for_bus_sync:%s",error->message);
        g_error_free(error);
    }else{
        g_signal_connect(customData.shellInterface,"reverse-message",G_CALLBACK(reverseMessage),NULL);
		//g_signal_connect(customData.shellInterface,"message",G_CALLBACK(Message),NULL);
    }
	


#if 1
	g_thread_new("camerainit",Camerainit,&customData);
    GThread *ioThread = g_thread_new("iodetect",iodetectRun,&customData);
#endif


    g_main_loop_run (loop);
//    g_thread_join(ioThread);

    /* Free resources */
    g_object_unref(customData.serialPortInterface);
    g_object_unref(customData.shellInterface);
    gst_object_unref (bus);
    g_source_remove (bus_watch_id);
    g_main_loop_unref(loop);
    return 0;
}
