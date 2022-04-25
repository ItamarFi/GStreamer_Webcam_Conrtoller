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

class CommandStop : public ICommand
{
public:
    CommandStop(VideoStream *stream);
    void execute() override;
};

class CommandPause : public ICommand
{
public:
    CommandPause(VideoStream *stream);
    void execute() override;
};

class CommandRecord : public ICommand
{
private:
    SubVideoStream* record;  

public:
    CommandRecord(VideoStream *stream);
    void execute() override;
};

class CommandDisplay : public ICommand
{
private:
    SubVideoStream* display;  

public:
    CommandDisplay(VideoStream *stream);
    void execute() override;
};

class CommandStatus : public ICommand
{
public:
    CommandStatus(VideoStream *stream);
    void execute() override;
};

class CommandConnectCam: public ICommand
{
private:
    SubVideoStream* webcam;  

public:
    CommandConnectCam(VideoStream *stream);
    void execute() override;
};

#endif
