#pragma once

#include "atmega128rfa1/core.h"
#include "atmega128rfa1/attr.h"

namespace armarow {
namespace drv {
    template<typename Config>
    struct Atmega128RFA1 : public atmega128rfa1::AttributeHandler< atmega128rfa1::Core<Config> >{};
}
}
