#pragma once

#include <boost/static_assert.hpp>

#include "error.h"

namespace armarow {
namespace common {

    template<typename MsgType>
    struct Interface
    {
        typedef MsgType MessageType;
        Error send(MessageType& msg)
        {
            BOOST_STATIC_ASSERT_MSG(!sizeof(MessageType), "not_implemented_yet");
            return FAILURE;
        }

        Error receive(MessageType& msg)
        {
            BOOST_STATIC_ASSERT_MSG(!sizeof(MessageType), "not_implemented_yet");
            return FAILURE;
        }

        template<typename AttributeContainer>
        Error getAttribute(AttributeContainer& attr) const
        {
            BOOST_STATIC_ASSERT_MSG(!sizeof(MessageType), "not_implemented_yet");
            return FAILURE;
        }

        template<typename AttributeContainer>
        Error setAttribute(const AttributeContainer& attr)
        {
            BOOST_STATIC_ASSERT_MSG(!sizeof(MessageType), "not_implemented_yet");
            return FAILURE;
        }

        void reset()
        {
            BOOST_STATIC_ASSERT_MSG(!sizeof(MessageType), "not_implemented_yet");
        }
    };
}
}
