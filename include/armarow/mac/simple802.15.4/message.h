#include "header.h"

namespace armarow {
namespace mac {
namespace simple802_15_4
{
    template<typename config> struct Properties{
        uint8_t  lqi;
        uint16_t crc;
    };
    template<typename config, typename BaseMessage> struct Message:
        public BaseMessage::template extend< FrameHeader<config>, Properties<config> >::type{
            typedef uint8_t SequenceNumberType;
        };
}
}
}