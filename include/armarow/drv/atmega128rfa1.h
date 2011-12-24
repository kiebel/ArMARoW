#pragma once

#include "atmega128rfa1/core.h"

namespace armarow {
namespace drv {
    template<typename Config>
    struct Atmega128RFA1 : public atmega128rfa1:Core<Config>{};
}
}
