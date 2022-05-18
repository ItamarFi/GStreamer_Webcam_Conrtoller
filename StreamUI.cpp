#include "StreamUI.hpp"

void StreamWindowUI::on_display_realize()
{
    // acquire an xwindow pointer to our draw area   
    window_handle = GDK_WINDOW_XID(video_window->get_window()->gobj() );
}

gboolean StreamWindowUI::on_display_draw()
{
    // if (data->state < GST_STATE_PAUSED)
    // {
    //     GtkAllocation allocation;

    //     gtk_widget_get_allocation(widget, &allocation);
    //     cairo_set_source_rgb(cr, 0, 0, 0);
    //     cairo_rectangle(cr, 0, 0, allocation.width, allocation.height);
    //     cairo_fill(cr);
    // }

    return FALSE;
}

static void cb_message (GstBus *bus, GstMessage *msg, VideoStream *data) {

    switch (GST_MESSAGE_TYPE (msg)) {

        case GST_MESSAGE_ERROR:
            GError *err;
            gchar *debug;

            gst_message_parse_error (msg, &err, &debug);
            g_print ("Error: %s\n", err->message);
            g_error_free (err);
            g_free (debug);

            gst_element_set_state ((GstElement*)data->getPipeline(), GST_STATE_READY);
            // g_main_loop_quit (data->loop);
            break;
        
        case GST_MESSAGE_WARNING:
            GError *gerror;
            gst_message_parse_warning (msg, &gerror, NULL);
            g_print("\t%s\n", gerror->message);
            g_print("\tSource of warning: %s (%s)\n", ((GstObject *)msg->src)->name, gst_element_get_name((GstElement*)gst_pad_get_parent((GstPad*)msg->src)));

            g_error_free(gerror);

            break;
        
        case GST_MESSAGE_STATE_CHANGED:
            // g_print("\t<<debug note: GST_MESSAGE_STATE_CHANGED activated>>\n");
            
            // GstState old_state, new_state;
            // gst_message_parse_state_changed (msg, &old_state, &new_state, NULL);
            // g_print ("\tElement %s changed state from %s to %s.\n",
            // GST_OBJECT_NAME (msg->src),
            // gst_element_state_get_name (old_state),
            // gst_element_state_get_name (new_state));
            break;


        case GST_MESSAGE_EOS:
            // g_print("\t<<debug note: GST_MESSAGE_EOS activated>>\n");

            /* end-of-stream */
            gst_element_set_state ((GstElement*)data->getPipeline(), GST_STATE_READY);
            // g_main_loop_quit (data->loop);
            break;
        
        case GST_MESSAGE_BUFFERING: {
            // g_print("\t<<debug note: GST_MESSAGE_BUFFERING activated>>\n");

            gint percent = 0;

            /* If the stream is live, we do not care about buffering. */
            //   if (data->is_live) break;

            gst_message_parse_buffering (msg, &percent);
            g_print ("Buffering (%3d%%)\r", percent);
            /* Wait until buffering is complete before start/resume playing */
            if (percent < 100)
                gst_element_set_state ((GstElement*)data->getPipeline(), GST_STATE_PAUSED);
            else
                gst_element_set_state ((GstElement*)data->getPipeline(), GST_STATE_PLAYING);
            break;
        }
        
        case GST_MESSAGE_CLOCK_LOST:

            /* Get a new clock */
            gst_element_set_state ((GstElement*)data->getPipeline(), GST_STATE_PAUSED);
            gst_element_set_state ((GstElement*)data->getPipeline(), GST_STATE_PLAYING);
            break;
            default:
            /* Unhandled message */
            break;
        
        case GST_MESSAGE_QOS:
            // g_print("Source of qos: %s (%s)\n", ((GstObject *)msg->src)->name, gst_element_get_name((GstElement*)gst_pad_get_parent((GstPad*)msg->src)));
            // gst_event_parse_qos
            
            break;


        case GST_MESSAGE_STREAM_STATUS:
            const GValue *gvalue = gst_message_get_stream_status_object (msg);
            gchar* strVal = g_strdup_value_contents (gvalue);
            // g_print ("\tgvalue: %s\n", strVal);
            free (strVal);
            break;        
    }
}


static GstBusSyncReply bus_sync_handler(GstBus     *bus,
                                        GstMessage *message,
                                        guintptr &window_handle)
{
    // Ignore all but prepare-window-handle messages
    if (!gst_is_video_overlay_prepare_window_handle_message(message))
    {
        return GST_BUS_PASS;
    }

    if (window_handle)
    {
        g_print("About to assign window to overlay\n");

        gst_video_overlay_set_window_handle(
            GST_VIDEO_OVERLAY(GST_MESSAGE_SRC(message)),
            window_handle);
    }
    else
    {
        g_warning("Should have gotten a video window handle by now\n");
    }
}



StreamWindowUI::StreamWindowUI(VideoStream *vs) :
    m_vbox(false, 5),
    m_play_button(Gtk::Stock::MEDIA_PLAY),
    m_pause_button(Gtk::Stock::MEDIA_PAUSE),
    m_stop_button(Gtk::Stock::MEDIA_STOP),
    m_record_button(Gtk::Stock::MEDIA_RECORD),
    m_display_button(Gtk::Stock::LEAVE_FULLSCREEN),
    m_vs(vs)
{
    set_default_size(300,300);
    set_title("GStreamer UI - Gtkmm/C++");
    set_position(Gtk::WIN_POS_CENTER);

    CommandConnectCam *connect_cam = new CommandConnectCam(m_vs);
    connect_cam->execute();

    m_vs_bus = gst_element_get_bus((GstElement *) m_vs->getPipeline());
    gst_bus_set_sync_handler(m_vs_bus, (GstBusSyncHandler)bus_sync_handler,
                             &window_handle, NULL);
    gst_bus_add_signal_watch(m_vs_bus);

    g_signal_connect (m_vs_bus, "message", G_CALLBACK (cb_message), &m_vs);


    std::vector<GstElement*> window_sink_elements;

    window_sink_elements.push_back(gst_element_factory_make("queue", "queue_window"));
    window_sink_elements.push_back(gst_element_factory_make("xvimagesink", "sink_window"));
    // "glimagesink"?


    const gchar *window_sink_name = "window";
    m_vs->addSubStream(new SubVideoStream(window_sink_elements, window_sink_name), GST_PAD_SINK);
    // overlay = (GstVideoOverlay*) (gst_bin_get_by_name ((GstBin *) m_vs->getPipeline(), "sink_window"));

    video_window = new Gtk::DrawingArea();
    video_window->signal_realize().connect( sigc::mem_fun( *this, &StreamWindowUI::on_display_realize ));
    video_window->set_double_buffered(false);
    // video_window->signal_draw().connect( sigc::mem_fun( *this, &StreamWindowUI::on_display_draw ));

    add(m_vbox);
    m_vbox.pack_start(*video_window);
    m_vbox.pack_start(m_button_box);
    m_button_box.pack_start(m_play_button);
    m_button_box.pack_start(m_pause_button);
    m_button_box.pack_start(m_stop_button);
    m_button_box.pack_start(m_record_button);
    m_button_box.pack_start(m_display_button);

    // set commands
    play = new CommandStart(m_vs);
    pause = new CommandPause(m_vs);
    stop = new CommandStop(m_vs);
    record = new CommandRecord(m_vs);
    frames = new CommandSaveAsFrames(m_vs);
    display = new CommandDisplay(m_vs);


    // set signals to buttons (signal -> execute)
    m_play_button.signal_clicked().connect(
    sigc::mem_fun(*this, &StreamWindowUI::on_button_play));
    m_pause_button.signal_clicked().connect(
    sigc::mem_fun(*this, &StreamWindowUI::on_button_pause));
    m_stop_button.signal_clicked().connect(
    sigc::mem_fun(*this, &StreamWindowUI::on_button_stop));
    m_record_button.signal_clicked().connect(
    sigc::mem_fun(*this, &StreamWindowUI::on_button_record));
    m_display_button.signal_clicked().connect(
    sigc::mem_fun(*this, &StreamWindowUI::on_button_display));

    show_all_children();
}

StreamWindowUI::~StreamWindowUI()
{
}

void StreamWindowUI::on_button_play()
{
    g_print("-------- PLAY --------\n");
    play->execute();
}

void StreamWindowUI::on_button_pause()
{
    g_print("-------- PAUSE --------\n");
    pause->execute();
}

void StreamWindowUI::on_button_stop()
{
    g_print("-------- STOP --------\n");
    stop->execute();
}

void StreamWindowUI::on_button_display()
{
    g_print("-------- DISPLAY --------\n");
    this->m_vs->printStatus();
    // display->execute();
}

void StreamWindowUI::on_button_record()
{
    g_print("-------- RECORD --------\n");
    // record->execute();
    frames->execute();
}
