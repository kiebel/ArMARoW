namespace PHY(DRV)
{
	struct MessageBuffer
	{
		struct
		{
			uint8_t size;
			uint8_t data[126];
		}dev;
		struct{ uint8_t size}* phyHead = &dev.size;
		uint8_t * payload = dev.data;
		const uint8_t payload_size = 126;

	}
}

namespace MAC{
	typedef PHY Super; //maybe templatepameter
	struct Message_Buffer: Super:Messsage_Buffer
	{
		struct{ uint8_t mybytes [16]}* macHead=this->Super:payload;
		uint8_t * payload = this->Super:payload+sizeof *macHead;
		const uint8_t payload_size = this->Super:payloadsize - sizeof *macHead;
	}
}

namespace RTE
{
	typedef MAC Super; //maybe templatepameter
	struct Message_Buffer: Super:Messsage_Buffer
	{
		struct{ uint8_t ttl}* rteHead = this->Super:payload;
		uint8_t * payload = this->Super:payload+sizeof *rteHead;
		const uint8_t payload_size = this->Super:payloadsize - sizeof *rteHead;
	}
}
