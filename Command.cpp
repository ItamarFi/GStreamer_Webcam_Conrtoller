#include "Command.hpp"

// commands:
// start                v
// stop                 v
// pause                v
// show elements?
// record               v
// display              v
// remove record
// remove display

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


class CommandStop : public ICommand
{
public:
    CommandStop(VideoStream *stream) : ICommand(stream) {
        this->mainstream = stream;
        this->description = "Stop playing";
    }
    void execute() {
        mainstream->stopPlaying();
    }
};

class CommandPause : public ICommand
{
public:
    CommandPause(VideoStream *stream) : ICommand(stream) {
        this->mainstream = stream;
        this->description = "Pause playing";
    }
    void execute() {
        mainstream->pausePlaying();
    }
};


class CommandConnectCam : public ICommand
{
private:
  SubVideoStream* webcam;  

public:
    CommandConnectCam(VideoStream *stream) : ICommand(stream) {
        this->mainstream = stream;
        this->description = "Connect to Webcam";
        
        std::vector<GstElement*> webcam_elements;
        webcam_elements.push_back(gst_element_factory_make ("v4l2src", "webcam_source"));
        
        const gchar *webcam_name = "webcam";
        this->webcam = new SubVideoStream(webcam_elements, webcam_name);
    }
    void execute() {
        this->mainstream->addSubStream(this->webcam, GST_PAD_SRC);
    }
};


class CommandRecord : public ICommand
{
private:
  SubVideoStream* record;  

public:
    CommandRecord(VideoStream *stream) : ICommand(stream) {
        this->mainstream = stream;
        this->description = "Start record";

        std::vector<GstElement*> record_elements;
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
    void execute() override {
        this->mainstream->addSubStream(this->record, GST_PAD_SINK);
    }
};

class CommandDisplay : public ICommand
{
private:
  SubVideoStream* display;  

public:
    CommandDisplay(VideoStream *stream) : ICommand(stream) {
        this->mainstream = stream;
        this->description = "Start display";

        std::vector<GstElement*> display_elements;
        display_elements.push_back(gst_element_factory_make("queue", "queue_display"));
        display_elements.push_back(gst_element_factory_make("xvimagesink", "sink_display"));

        const gchar *display_name = "display";
        this->display = new SubVideoStream(display_elements, display_name);
    }
    void execute() {
        this->mainstream->addSubStream(this->display, GST_PAD_SINK);
    }
};

class CommandStatus : public ICommand
{
private:
  SubVideoStream* display;  

public:
    CommandStatus(VideoStream *stream) : ICommand(stream) {
        this->mainstream = stream;
        this->description = "Print status of VideoStream";
    }
    void execute() {
        g_print("\n===========================\n");
        g_print("Status:\n");
        this->mainstream->printStatus();
        g_print("===========================\n");
    }
};