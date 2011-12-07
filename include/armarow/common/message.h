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

    template< typename ConcatHeaders, typename ConcatAttrs, typename PayloadType >
    struct assembleMessage
    {
        struct type : public ConcatHeaders, public PayloadType, public ConcatAttrs 
        {
            private:
                typedef type Message;
                typedef ConcatHeaders Headers;
                typedef ConcatAttrs Attributes;

            public:
                static const uint8_t maxSize = sizeof(Headers) + sizeof(PayloadType);
            
            template<typename NewHeader, typename NewAttributes>
            struct extend
            {
                private:
                    typedef typename concat<Headers, NewHeader>::type        ExtHeader;
                    typedef typename concat<Attributes, NewAttributes>::type ExtAttr;
                    typedef typename PayloadType::template resize< 
                                        maxSize - sizeof( ExtHeader) >::type  NewPayloadType;
                public:
                    struct type : public assembleMessage< ExtHeader, 
                                                          ExtAttr, 
                                                          NewPayloadType
                                                        >::type
                    {

                        type(){}
                        type(Message& copy){}
                        type& operator =(const Message& copy){return *reinterpret_cast<type*>(&copy);}       
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

    struct DefaultAttr
    {
        MessageState state;

        DefaultAttr() : state(NOTHING) {};
    };

    template<uint8_t size>
    struct Payload
    {
        uint8_t data[size];

        template<uint8_t newSize>
        struct resize
        {
            typedef Payload<newSize> type;
        };
    };

    template<uint8_t maxSize>
    struct Message : public meta::assembleMessage< DefaultHeader,
                                                   DefaultAttr,
                                                   Payload< maxSize -sizeof(DefaultHeader) >
                                                 >::type
    {};
}
}
