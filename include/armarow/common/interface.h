#pragma once

#include <boost/static_assert.hpp>

#include "error.h"

namespace armarow {
namespace common {

	template<typename Message>
	class Interface
	{
		public:
			Error send(Message& msg)
			{
				BOOST_STATIC_ASSERT_MSG(!sizeof(Message), "not_implemented_yet");
				return FAILURE;
			}

			Error receive(Message& msg)
			{
				BOOST_STATIC_ASSERT_MSG(!sizeof(Message), "not_implemented_yet");
				return FAILURE;
			}

			template<typename AttributeContainer>
			Error getAttribute(AttributeContainer& attr) const
			{
				BOOST_STATIC_ASSERT_MSG(!sizeof(Message), "not_implemented_yet");
				return FAILURE;
			}

			template<typename AttributeContainer>
			Error setAttribute(const AttributeContainer& attr)
			{
				BOOST_STATIC_ASSERT_MSG(!sizeof(Message), "not_implemented_yet");
				return FAILURE;
			}
	};
}
}
