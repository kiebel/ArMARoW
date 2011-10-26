#include <boost/mpl/vector.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/push_back.hpp>
#include <stdint.h>

namespace meta
{
	template<typename types>
	struct orderedMerge
	{
		private:
			struct empty {};

			struct merge
			{
				template<typename folded, typename next>
				struct apply
				{
					struct type : public folded, public next {};
				};
			};
		public:
			typedef typename boost::mpl::fold< types, 
											   empty,
											   merge 
							 >::type type;
	};

	template<typename Headers, template<uint8_t> class PayloadType, uint8_t maxPayload>
	struct assembleMessage
	{
		private:
			struct sum
			{
				template<typename folded, typename next>
				struct apply
				{
					typedef typename boost::mpl::int_< folded::value + sizeof(next) 
						    >::type type;
				};
			};

			static const uint8_t partPayload = boost::mpl::fold< Headers, 
																 boost::mpl::int_<0>::type,
																 sum
											   >::type::value;

			typedef PayloadType< maxPayload - partPayload > Payload;

			typedef typename boost::mpl::push_back< Headers, Payload >::type MessageParts;
		public:
			typedef typename orderedMerge< MessageParts >::type type;
	};
}

namespace common
{
	template<uint8_t size>
	struct Payload
	{
		uint8_t payload[size];
	};
	typedef boost::mpl::vector<>::type HeaderList;
}

namespace phy
{
	static const uint8_t maxPayload=128;

	struct PhyHeader
	{
		uint8_t size;
	};
	
	typedef boost::mpl::push_back<common::HeaderList, PhyHeader>::type  HeaderList;

	typedef meta::assembleMessage< 
				HeaderList,
				common::Payload,
				phy::maxPayload
			>::type Message;
}

namespace mac
{
	struct MacHeader
	{
		uint8_t src;
		uint8_t dest;
	};

	typedef boost::mpl::push_back<phy::HeaderList, MacHeader>::type HeaderList;
	
	typedef meta::assembleMessage< 
				HeaderList,
				common::Payload,
				phy::maxPayload
			>::type Message;
}

namespace routing
{
	struct RoutingHeader
	{
		uint8_t nextHop;
		uint8_t ttl;
	};

	typedef boost::mpl::push_back<mac::HeaderList, RoutingHeader>::type HeaderList;
	
	typedef meta::assembleMessage< 
				HeaderList,
				common::Payload,
				phy::maxPayload
			>::type Message;
}
