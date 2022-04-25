// ./p2
// gcc *.cpp -o p2 -lm -lstdc++ `pkg-config --cflags --libs gtk+-3.0 gstreamer-1.0 gtkmm-3.0 gstreamer-video-1.0 gobject-2.0 glib-2.0`
#include <gtk/gtk.h>
#include <gst/gst.h>
#include "VideoStream.hpp"
#include "StreamUI.hpp"

#include <gtkmm.h>

struct _CustomData {
  VideoStream *vs;
};


int main (int argc, char **argv)
{
  /* Initialize GStreamer, GKT+ */
  gst_init (&argc, &argv);
  auto app = Gtk::Application::create(argc, argv, "gstreamer.gui.gtk.tutorial");

  /* Create data */  
  _CustomData data; 
  data.vs = new VideoStream();

  StreamWindowUI window(data.vs);

  return app->run(window);

}
