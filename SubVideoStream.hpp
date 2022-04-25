#ifndef SUBVIDEOSTREAM_H
#define SUBVIDEOSTREAM_H

#include <gst/gst.h>
#include <iostream>
#include <vector>
#include <algorithm>


class SubVideoStream
{
private:
    std::vector<GstElement*> element;
    GstBin* bin;
    
public:
    SubVideoStream(std::vector<GstElement*> elements_vec, const gchar* name);
    ~SubVideoStream();
    // GstPad* linkAsSource();
    // GstPad* linkAsSink();

    GstBin *getBin();
    GstPad* getLinkPad(GstPadDirection direction);
    const gchar* getName();
    // void start();
    // void stop(); 

};

// class DisplayStream : public SubVideoStream {};

// class RecordStream : public SubVideoStream {};

#endif
