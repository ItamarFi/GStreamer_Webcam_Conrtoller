#ifndef COMMANDKEY_H
#define COMMANDKEY_H

#include <vector>
#include <gst/gst.h>
#include "VideoStream.hpp"


class CommandKey
{
private:
    const gchar* key_holder;
public:
    CommandKey(const gchar* key);
    ~CommandKey();
    const gchar* getKey();
};

struct CommandKeyComparator;
// {
//     bool operator()(CommandKey* &a, CommandKey* &b) const {
//         return g_strcmp0(a->getKey(), b->getKey());
//     }
// };

#endif