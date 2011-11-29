#pragma once

//#include <boost/static_assert.hpp>

#include "armarow/common/error.h"

namespace armarow {
namespace mac {

	using common::Error;

	class MacInterface
	{
		public:
			template<typename Message>
			Error send(Message& msg)
			{
//				BOOST_STATIC_ASSERT_MSG(false, "not_implemented_yet");
				return common::FAILURE;
			}

			template<typename Message>
			Error receive(Message& msg)
			{
//				BOOST_STATIC_ASSERT_MSG(false, "not_implemented_yet");
				return common::FAILURE;
			}

			template<typename AttributeContainer>
			Error getAttribute(AttributeContainer& attr) const
			{
//				BOOST_STATIC_ASSERT_MSG(false, "not_implemented_yet");
				return common::FAILURE;
			}

			template<typename AttributeContainer>
			Error setAttribute(const AttributeContainer& attr)
			{
//				BOOST_STATIC_ASSERT_MSG(false, "not_implemented_yet");
				return common::FAILURE;
			}
	};
}
}
