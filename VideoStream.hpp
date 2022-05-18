#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include <gst/gst.h>
#include <vector>
#include "SubVideoStream.hpp"
#include <unistd.h>


class VideoStream
{
private:
    GstElement *pipeline, *tee;
    GstBus *bus;
    bool stop;
    void linkPads(GstPad *pad);

public:
    VideoStream();
    ~VideoStream();
    void startPlaying();
    void stopPlaying();
    void pausePlaying();
    void addSubStream(SubVideoStream *sub, GstPadDirection direction);
    void removeSubStream(SubVideoStream sub);
    void printStatus();
    void printChildrenElementsState();
    GstBus* getBus();
    GstElement* getPipeline();

};

#endif
