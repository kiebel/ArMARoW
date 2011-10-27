#include <stdint.h>

namespace common
{
	template<uint8_t size>
	struct Payload
	{
		uint8_t payload[size];
	};

	struct Empty{};
	struct Empty_c{};
	
	template<typename Header,int Size,typename Attribute>
	class Message:
		public Header,
		public Payload<Size-sizeof(Header)>,
		public Attribute
	{};
	
	
}

namespace phy
{
	static const uint8_t maxPayload=128;

	template<typename prev> struct PhyHeader : public prev
	{
		uint8_t size;
	};
	
	typedef PhyHeader<common::Empty> Header;
	typedef common::Message<Header,phy::maxPayload,common::Empty_c> Message;
	
}

namespace mac
{
	template<typename prev> struct MacHeader: public prev
	{
		uint8_t src;
		uint8_t dest;
	};
	
	typedef MacHeader<phy::Header> Header;
	typedef common::Message<Header,phy::maxPayload,common::Empty_c> Message;
}

namespace routing
{
	template<typename prev> struct RoutingHeader: public prev
	{
		uint8_t nextHop;
		uint8_t ttl;
		uint8_t getdst(){return this->dest;}
	};

	typedef RoutingHeader<mac::Header> Header;
	typedef common::Message<Header,phy::maxPayload,common::Empty_c> Message;
	
}


