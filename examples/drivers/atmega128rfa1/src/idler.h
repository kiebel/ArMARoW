#pragma once

#include <platform.h>

namespace platform {
namespace deRCB128RFA1 
{
    struct Idler
    {
        static void idle()
        {
            while(true)
                Morpheus::sleep<Morpheus::SleepModes::idle>();
        }
    };
}
}
