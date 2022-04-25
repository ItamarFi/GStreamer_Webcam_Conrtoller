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
    g_print("<<bus_sync_handler>>\n");
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

    std::vector<GstElement*> window_sink_elements;
    glimagesink = gst_element_factory_make("glimagesink", "window_sink");
    window_sink_elements.push_back(glimagesink);
    const gchar *window_sink_name = "window_sink";
    m_vs->addSubStream(new SubVideoStream(window_sink_elements, window_sink_name), GST_PAD_SINK);
    overlay = (GstVideoOverlay*) (gst_bin_get_by_name ((GstBin *) m_vs->getPipeline(), "window_sink"));

    video_window = new Gtk::DrawingArea();
    video_window->signal_realize().connect( sigc::mem_fun( *this, &StreamWindowUI::on_display_realize ));
    // video_window->signal_draw().connect( sigc::mem_fun( *this, &StreamWindowUI::on_display_draw ));

    add(m_vbox);
    m_vbox.pack_start(*video_window);
    m_vbox.pack_start(m_button_box);
    m_button_box.pack_start(m_play_button);
    m_button_box.pack_start(m_pause_button);
    m_button_box.pack_start(m_stop_button);
    m_button_box.pack_start(m_record_button);
    // m_button_box.pack_start(m_display_button);

    // set commands
    play = new CommandStart(m_vs);
    pause = new CommandPause(m_vs);
    stop = new CommandStop(m_vs);
    record = new CommandRecord(m_vs);
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
    play->execute();
}

void StreamWindowUI::on_button_pause()
{
    pause->execute();
}

void StreamWindowUI::on_button_stop()
{
    stop->execute();
}

void StreamWindowUI::on_button_display()
{
    display->execute();
}

void StreamWindowUI::on_button_record()
{
    record->execute();
}