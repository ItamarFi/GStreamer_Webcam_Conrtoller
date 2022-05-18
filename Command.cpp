#include "Command.hpp"

ICommand::ICommand(VideoStream *stream) {
    this->mainstream = stream;
}

ICommand::~ICommand() {
    
};

const gchar* ICommand::getDescription() {
    return this->description;
};

CommandStart::CommandStart (VideoStream *stream) : ICommand(stream) {
    this->mainstream = stream;
    this->description = "Start playing";
}

void CommandStart::execute() {
    this->mainstream->startPlaying();
}

CommandStop::CommandStop (VideoStream *stream) : ICommand(stream) {
    this->mainstream = stream;
    this->description = "Stop playing";
    }

void CommandStop::execute() {
      mainstream->stopPlaying();
}

CommandPause::CommandPause (VideoStream *stream) : ICommand(stream) {
    this->mainstream = stream;
    this->description = "Pause playing";
    }

void CommandPause::execute() {
      mainstream->pausePlaying();
}

CommandConnectCam::CommandConnectCam (VideoStream *stream) : ICommand(stream) {
      this->mainstream = stream;
      this->description = "Connect to Webcam";
        
      std::vector<GstElement*> webcam_elements;
      // nvarguscamerasrc / nvgstcapture
      // webcam_elements.push_back(gst_element_factory_make ("nvarguscamerasrc", "webcam_source"));
      webcam_elements.push_back(gst_element_factory_make ("v4l2src", "webcam_source"));
      
      const gchar *webcam_name = "webcam";
      this->webcam = new SubVideoStream(webcam_elements, webcam_name);
    }

void CommandConnectCam::execute() {
    this->mainstream->addSubStream(this->webcam, GST_PAD_SRC);
}

CommandRecord::CommandRecord  (VideoStream *stream) : ICommand(stream) {
      this->mainstream = stream;
      this->description = "Start record";

      std::vector<GstElement*> record_elements;

      // record_elements.push_back(gst_element_factory_make("queue", "queue_record"));
      // record_elements.push_back(gst_element_factory_make("x264enc", NULL));
      // record_elements.push_back(gst_element_factory_make("mp4mux",  NULL));
      // record_elements.push_back(gst_element_factory_make ("filesink", "sink_record"));
	    // g_object_set(record_elements[2], "location", "record.mp4", NULL);
      // g_object_set(record_elements[1], "tune", 4, NULL);



      // record_elements.push_back(gst_element_factory_make("queue", "queue_record"));
      // record_elements.push_back(gst_element_factory_make("nvdsosd", NULL));
      // record_elements.push_back(gst_element_factory_make("nvvideoconvert", NULL));
      // record_elements.push_back(gst_element_factory_make("video/x-raw(memory:NVMM),format=NV12", NULL));
      // record_elements.push_back(gst_element_factory_make("nvv4l2h264enc", NULL));
      // record_elements.push_back(gst_element_factory_make("h264parse", NULL));
      // record_elements.push_back(gst_element_factory_make("qtmux", NULL));
      // record_elements.push_back(gst_element_factory_make("filesink", "sink_record"));
	    // g_object_set(record_elements[7], "location", "record.mp4", NULL);

      // TODO: find the right way to stream and record avoiding buffers and qos. check pads capabilities. nvcamerasrc vs v4l2src. integration with Jeston Nano.
      record_elements.push_back(gst_element_factory_make("queue", "queue_record"));
      record_elements.push_back(gst_element_factory_make("videoconvert", NULL));
      record_elements.push_back(gst_element_factory_make("videoscale", NULL));
      record_elements.push_back(gst_element_factory_make("videorate", NULL));
      record_elements.push_back(gst_element_factory_make("theoraenc", NULL));
      record_elements.push_back(gst_element_factory_make("oggmux", NULL));
      record_elements.push_back(gst_element_factory_make ("filesink", "sink_record"));
	    g_object_set(record_elements[6], "location", "record.mp4", NULL);

      const gchar *record_name = "record";
      this->record = new SubVideoStream(record_elements, record_name);
  }

void CommandRecord::execute() {
  // g_print("\nDebug: Record\n");
    this->mainstream->addSubStream(this->record, GST_PAD_SINK);
}

CommandDisplay::CommandDisplay(VideoStream *stream) : ICommand(stream) {
        this->mainstream = stream;
        this->description = "Start display";

        std::vector<GstElement*> display_elements;
        display_elements.push_back(gst_element_factory_make("queue", "queue_display"));
        display_elements.push_back(gst_element_factory_make("xvimagesink", "sink_display"));
        // display_elements.push_back(gst_element_factory_make("glimagesink", "sink_display"));

        const gchar *display_name = "display";
        this->display = new SubVideoStream(display_elements, display_name);
    }

void CommandDisplay::execute() {
        this->mainstream->addSubStream(this->display, GST_PAD_SINK);
}

CommandSaveAsFrames::CommandSaveAsFrames (VideoStream *stream) : ICommand(stream) {
        this->mainstream = stream;
        this->description = "Save stream as separated frames in JPEG files";

        std::vector<GstElement*> frames_elements;
        frames_elements.push_back(gst_element_factory_make("queue", "queue_frames"));
        frames_elements.push_back(gst_element_factory_make("jpegenc", NULL));
        frames_elements.push_back(gst_element_factory_make("multifilesink", NULL));
        g_object_set(frames_elements[2], "location", "frame_%06d.jpg", NULL);

        const gchar *frames_name = "frames";
        this->frames = new SubVideoStream(frames_elements, frames_name);

}

void CommandSaveAsFrames::execute() {
        this->mainstream->addSubStream(this->frames, GST_PAD_SINK);
}

CommandStatus::CommandStatus(VideoStream *stream) : ICommand(stream) {
    this->mainstream = stream;
    this->description = "Print status of VideoStream";
}
void CommandStatus::execute() {
        g_print("%s", "\n===========================\n");
        g_print("%s", "Status:\n");
        this->mainstream->printStatus();
        g_print("%s", "===========================\n");
}

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
  
  g_printerr("%s", "\nList of available devices:\n\n");

  // iterate over the list and print the names of the devices
  while (devices != NULL)
  {
    device_name = gst_device_get_display_name((GstDevice *)devices->data);
    g_print("%s", device_name);
    g_print("%s", "\n");

    devices = devices->next;
  }
  g_print("%s", "\n");

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
    printf("%s", "WARNING: Monitor couldn't started.\n");
  }

  GList *devices = gst_device_monitor_get_devices(monitor);
  gchar *device_name;
  gchar *device_port;
  std::string d_str;

// print title
  g_print("%s", "Devices found with substring \"");
  g_print("%s", name.c_str());
  g_print("%s", "\":\n\n");

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

      g_print("%s", "Device name: ");
      g_print("%s", device_name);
      g_print("%s", "\n");

      g_print("%s", "Device port: ");
      g_print("%s", gst_structure_get_string(gstruct, field_name));
      g_print("%s", "\n\n");

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
    g_print("%s", "Sorry. Device not found.\n");
  }
}
