#pragma once

#include <stdint.h>
#include "messageState.h"

namespace armarow {
namespace meta {

    template<typename Extension, typename Base>
    struct concat
    {
        struct type : public Base, public Extension {};
    };

    template< typename ConcatHeaders, typename ConcatProps, uint16_t maximumSize >
    struct assembleMessage
    {
        struct type
        {
            private:
                typedef type Message;
                typedef ConcatHeaders Header;
                typedef ConcatProps Properties;

            public:
                static const uint16_t maxSize = maximumSize;

                Header     header;
                uint8_t    payload[maxSize - sizeof(ConcatHeaders)];
                Properties properties;

                static const uint16_t payloadSize = sizeof(payload);

                template<typename NewHeader, typename NewProps>
                struct extend
                {
                    private:
                        typedef typename concat<Header, NewHeader>::type    ExtHeader;
                        typedef typename concat<Properties, NewProps>::type ExtProps;
                    public:
                        struct type : public assembleMessage< ExtHeader, 
                                                              ExtProps, 
                                                              maxSize
                                                            >::type
                        {};
                };

                template<typename CustomType>
                struct customize
                {
                    struct type
                    {
                        static const uint16_t maxSize = maximumSize;

                        Header     header;
                        CustomType payload;
                        Properties properies;

                        static const uint16_t payloadSize = sizeof(payload);
                    };
                };
        };
    };
}

namespace common{

    struct DefaultHeader
    {
        uint8_t size;

        DefaultHeader() : size(0){}
    };

    struct DefaultProps
    {
        MessageState state;

        DefaultProps() : state(NOTHING) {};
    };

    template<uint16_t maxSize>
    struct Message : public meta::assembleMessage< DefaultHeader,
                                                   DefaultProps,
                                                   maxSize >::type
    {};
}
}
