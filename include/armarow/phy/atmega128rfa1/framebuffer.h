#pragma once

#include <avr-halib/regmaps/base/localRegMap.h>
#include <stdint.h>

namespace avr_halib
{
namespace regmaps
{
namespace local
{
namespace atmega128rfa1
{

/** \brief Regmap for radio framebuffer **/
class trxframebuffer : public base::LocalRegMap
{
public:
	union
	{
		struct
		{
			uint8_t __padrxlength[0x17b];
			uint8_t rxlength;
		};
		struct
		{
			uint8_t __padtxfb[0x180];
			uint8_t trxfb[128];
		};
	};
}__attribute__((packed));
}
}
}
}
