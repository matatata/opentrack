#pragma once
#include <QString>
#include "options/options.hpp"

using namespace options;

struct osc_settings : opts
{
    value<QString> address;
    value<int> port;
    value<bool> emitQuaternion;
    value<bool> emitHeadpose;

    osc_settings() : opts("proto-osc"), address{b, "address", "127.0.0.1"},
        port(b, "port", 53101), emitHeadpose{b, "headpose",false}, emitQuaternion{b, "quaternion",true}
    {}
};
