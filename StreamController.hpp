#ifndef STREAMCONTROLLER_H
#define STREAMCONTROLLER_H

#include <gst/gst.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <map>

#include "VideoStream.hpp"
#include "SubVideoStream.hpp"
#include "Command.hpp"
#include "Command.cpp"
#include "CommandKey.hpp"


class StreamController
{
private:
    std::map<CommandKey*, ICommand*> commands;
    pthread_t p;
    VideoStream* mainstream;
    void InitBasicCommands();

public:
    StreamController(VideoStream* stream);
    ~StreamController();
    void CommandsList();
    static void* CommandsHandler(void *ptr);
    // AddCommand(gchar c, ICommand command);
};

#endif