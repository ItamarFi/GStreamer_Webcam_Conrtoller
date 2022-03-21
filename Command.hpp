#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <gst/gst.h>
#include "VideoStream.hpp"

class ICommand
{
protected:
    VideoStream *mainstream;
    const gchar *description;

public:
    ~ICommand();
    ICommand(VideoStream *stream);
    virtual void execute() = 0;
    const gchar* getDescription();
};

class CommandStart : public ICommand
{
public:
    CommandStart(VideoStream *stream);
    void execute() override;
};

// class CommandStatus : public ICommand
// {
// public:
//     CommandStatus(VideoStream *stream);
//     void execute() override;
// };

#endif