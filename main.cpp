#include <gst/gst.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "VideoStream.hpp"
#include "SubVideoStream.hpp"
#include "StreamController.hpp"
#include "StreamController.cpp"


/*
print list of devices 
*/
std::string printDevicesNames () {
  GstDeviceMonitor* monitor = gst_device_monitor_new();
  if (!gst_device_monitor_start(monitor)){
    printf("WARNING: Monitor couldn't started.\n");
  }

  // get list of GstDevice*
  GList *devices = gst_device_monitor_get_devices(monitor);
  gchar *device_name;
  
  g_printerr("\nList of available devices:\n\n");

  // iterate over the list and print the names of the devices
  while (devices != NULL)
  {
    device_name = gst_device_get_display_name((GstDevice *)devices->data);
    g_print(device_name);
    g_print("\n");

    devices = devices->next;
  }
  g_print("\n");

  // free resources
  devices = g_list_first(devices);
  g_list_free_full(devices, NULL);
  gst_device_monitor_stop(monitor);
}

/*
get name (full or particial) of device and return its port
*/
void findSourcePortByName (std::string name) {

  bool found = FALSE;

  GstDeviceMonitor* monitor = gst_device_monitor_new();
  if (!gst_device_monitor_start(monitor)){
    printf("WARNING: Monitor couldn't started.\n");
  }

  GList *devices = gst_device_monitor_get_devices(monitor);
  gchar *device_name;
  gchar *device_port;
  std::string d_str;

// print title
  g_print("Devices found with substring \"");
  g_print(name.c_str());
  g_print("\":\n\n");

// iterate over the list and look for compatible devices
  while (devices != NULL)
  {
    // compare names
    device_name = gst_device_get_display_name((GstDevice *)devices->data);
    if (d_str.assign(device_name).find(name) != std::string::npos) {
      // success. print details 
      found = TRUE;
      GstStructure *gstruct = gst_device_get_properties((GstDevice *)devices->data);
      const gchar *field_name = "device.path";

      g_print("Device name: ");
      g_print(device_name);
      g_print("\n");

      g_print("Device port: ");
      g_print(gst_structure_get_string(gstruct, field_name));
      g_print("\n\n");

      gst_structure_free(gstruct);
    }
    devices = devices->next;
  }

  // free resources
  devices = g_list_first(devices);
  g_list_free_full(devices, NULL);
  gst_device_monitor_stop(monitor);

  // device not found notice
  if (!found) {
    g_print("Sorry. Device not found.\n");
  }
}

/* ------- Main ------- */ 
int main (int argc, char *argv[])
{
  fflush(stdin);
  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  // printDevicesNames();
  // findSourcePortByName("Int");

  VideoStream *video_stream = new VideoStream();
  StreamController *stream_controller = new StreamController(video_stream);

  while(TRUE){}

}