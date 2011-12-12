#pragma once

#include <config.h>
#include "logging.h"

#include <avr-halib/avr/sleep.h>

namespace platform
{
namespace avr
{
namespace power
{
    struct Idler
    {
        typedef boost::mpl::list<logging::Sync> SyncList;
        typedef avr_halib::power::Morpheus<SyncList> Morpheus;
    
        static void idle(){
            while(true)
                Morpheus::sleep<config::sleepMode>();
        }
    };
}
}
}
