#pragma once

#include <stdint.h>
#include "messageState.h"

namespace armarow {
namespace meta {

    template<typename Extension, typename Base>
    struct concat
    {
        struct type : public Extension, public Base {} __attribute__((packed));
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

                template<class> friend class Extensible;
                template<class> friend class Customizable;
        } __attribute__((packed));
    };

    template<typename BaseMsg>
    struct Customizable
    {
        template<typename CustomType>
        struct customize
        {
            struct type
            {
                typedef BaseMsg BaseMessage;
                static const uint16_t maxSize = sizeof(CustomType);

                typename BaseMsg::Header     header;
                CustomType payload;
                typename BaseMsg::Properties properties;

                static const uint16_t payloadSize = sizeof(payload);

                static BaseMessage& down(type& from)
                {
                    from.header.size=sizeof(CustomType);
                    return *reinterpret_cast<BaseMessage*>(&from);
                }
                static type& up(BaseMessage& from)
                {
                    return *reinterpret_cast<type*>(&from);
                }
            } __attribute__((packed));
        };
    };

    template<typename BaseMsg>
    struct Extensible
    {
        template<typename NewHeader, typename NewProps>
        struct extend
        {
            private:
                typedef typename concat<typename BaseMsg::Header    , NewHeader>::type ExtHeader;
                typedef typename concat<typename BaseMsg::Properties, NewProps >::type ExtProps;
            public:
                struct type : public assembleMessage< ExtHeader, 
                                                      ExtProps, 
                                                      BaseMsg::maxSize
                                                    >::type,
                              public Extensible< type >,
                              public Customizable< type >
                {
                    typedef BaseMsg BaseMessage;
                    static BaseMessage& down(type& from)
                    {
                        from.header.size += sizeof(NewHeader);
                        return *reinterpret_cast<BaseMessage*>(&from);
                    }
                    static type& up(BaseMessage& from)
                    {
                        from.header.size -= sizeof(NewHeader);
                        return *reinterpret_cast<type*>(&from);
                    }
                } __attribute__((packed));
        };
    };
}

namespace common{

    struct DefaultHeader
    {
        uint8_t size;

        DefaultHeader() : size(0){}
    } __attribute__((packed));

    struct DefaultProps
    {
        MessageState state;

        DefaultProps() : state(NOTHING) {};
    } __attribute__((packed));

    template<uint16_t maxSize>
    struct Message : public meta::assembleMessage< DefaultHeader,
                                                   DefaultProps,
                                                   maxSize >::type, 
                     public meta::Extensible< Message< maxSize > >,
                     public meta::Customizable< Message< maxSize > >
    {};
}
}
