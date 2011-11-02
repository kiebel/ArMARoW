#include <stdint.h>

namespace common
{
	template<uint8_t size>
	struct Payload
	{
		uint8_t payload[size];
	};

	struct Empty_Header{};
	struct Empty_Attributes{};
	
	template<typename Header,int Size,typename Attribute>
	class Message:
		public Header,
		public Payload<Size-sizeof(Header)>,
		public Attribute
	{};
	
	template<typename AddHeader,typename PrevHeader>
	class Header:
		public AddHeader,
		public PrevHeader
	{};
	
	
	
}

namespace phy
{
	static const uint8_t maxPayload=128;

	struct PhyHeader
	{
		uint8_t size;
	};
	
	typedef common::Header<PhyHeader,common::Empty_Header> Header;
	typedef common::Message<Header,phy::maxPayload,common::Empty_Attributes> Message;
	
}

namespace mac
{
	struct MacHeader
	{
		uint8_t src;
		uint8_t dest;
	};
	
	typedef common::Header<MacHeader,phy::Header> Header;
	typedef common::Message<Header,phy::maxPayload,common::Empty_Attributes> Message;
}

namespace routing
{
	struct RoutingHeader
	{
		uint8_t nextHop;
		uint8_t ttl;
// 		uint8_t getdst(){return this->dest;}
	};

	typedef common::Header<RoutingHeader,mac::Header> Header;
	typedef common::Message<Header,phy::maxPayload,common::Empty_Attributes> Message;
	
}


