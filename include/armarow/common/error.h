#pragma once

#include <platform.h>

namespace armarow {
namespace common {

    /** \brief central definition of all possible error codes
     * 
     *  this enumeration has to be updated by the implementer of new functionality
     **/
    enum Error
    {
        SUCCESS = 0 , /**< operation successfull **/
        FAILURE     , /**< operation failed **/
        BUSY        , /**< medium or hardware busy **/
        OUT_OF_RANGE, /**< parameter not valid for this attribute **/
        NO_MESSAGE    /**< no message to transmit**/
    };
}
}

/** \brief specially overloaded operator to print readable strings of error codes
 *
 *  \param out the logging instance to report the error string to
 *  \param error the error the should be logged
 *  \return the logging instance passed as parameter
 **/
::LoggingOutput& operator<<(::LoggingOutput& out, const armarow::common::Error error)
{
    switch(error)
    {
        case(armarow::common::SUCCESS)     : return out << PROGMEMSTRING("ok");
        case(armarow::common::FAILURE)     : return out << PROGMEMSTRING("operation failed");
        case(armarow::common::BUSY)        : return out << PROGMEMSTRING("medium or hardware busy");
        case(armarow::common::OUT_OF_RANGE): return out << PROGMEMSTRING("parameter out of range");
        case(armarow::common::NO_MESSAGE)  : return out << PROGMEMSTRING("none Message recived");
    }
    return out << PROGMEMSTRING("unknown error");
}
