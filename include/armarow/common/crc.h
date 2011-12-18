#pragma once

#include <boost/mpl/int.hpp>

namespace armarow{
namespace common{
    struct CRC{
        struct Polynomials{
            typedef boost::mpl::int_<0x1021> ITU_T;
        };

        template<typename Polynom>
        static uint16_t calculateCRC( uint8_t* data, uint8_t length ){

            uint16_t crc = 0;
            for(uint8_t i=0;i<length-2;i++){
                crc ^= ( (uint16_t) data[i] << 8);

                for (uint8_t j=0; j<8; j++){
                    if( crc & 0x8000 )
                        crc = (crc << 1) ^ Polynom::value;
                    else
                        crc <<= 1;
                }
            }

            return crc;
        }
    };
};
};
