#ifndef STREAMUI_H
#define STREAMUI_H

#include <gtkmm.h>
#include <gst/video/videooverlay.h>
// #include <gdk/gdkx.h>
// #include <gdk/gdk.h>

#include <gdk/gdk.h>
#if defined (GDK_WINDOWING_X11)
#include <gdk/gdkx.h>
#elif defined (GDK_WINDOWING_WIN32)
#include <gdk/gdkwin32.h>
#elif defined (GDK_WINDOWING_QUARTZ)
#include <gdk/gdkquartz.h>
#endif

#include "VideoStream.hpp"
#include "SubVideoStream.hpp"
#include "Command.hpp"

class StreamWindowUI : public Gtk::Window
{
private:
    /* data */
public:
    StreamWindowUI(VideoStream *m_vs);
    virtual ~StreamWindowUI();
protected:
    // signal handlers
    void on_button_play();
    void on_button_pause();
    void on_button_stop();
    void on_button_record();
    void on_button_display();
    void on_display_realize();
    gboolean on_display_draw();

    // commands
    CommandStart *play;
    CommandPause *pause;
    CommandStop *stop;
    CommandRecord *record;
    CommandDisplay *display;

    // UI
    Gtk::VBox m_vbox;
    // Gtk::Window screen;
    Gtk::DrawingArea* video_window;
    GstVideoOverlay* overlay;
    Gtk::HButtonBox m_button_box;
    Gtk::Button m_play_button;
    Gtk::Button m_pause_button;
    Gtk::Button m_stop_button;
    Gtk::Button m_record_button;
    Gtk::Button m_display_button;

    // VideoStream - contains Gstreamer elements
    VideoStream *m_vs;
    guintptr window_handle;
    GstElement *glimagesink;
    GstBus *m_vs_bus;
};


#endif