#include "CommandKey.hpp"

CommandKey::CommandKey(const gchar* key)
{
    this->key_holder = key;
}

CommandKey::~CommandKey()
{
}

const gchar* CommandKey::getKey()
{
    return this->key_holder;
}

struct CommandKeyComparator
{
    bool operator()(CommandKey* &a, CommandKey* &b) const {
        int cmp = g_strcmp0(a->getKey(), b->getKey());
        return (cmp == 0);
    }
};
