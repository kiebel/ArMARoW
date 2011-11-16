#pragma once

//HALIB includes
#include <avr-halib/avr/clock.h>            //FIXME is this include used
#include <avr-halib/avr/regmaps.h>          //FIXME is this include used
#include <avr-halib/avr/timer.h>            //FIXME is this include used
#include <avr-halib/regmaps/local.h>        //FIXME is this include used
#include <avr-halib/share/delay.h>          //FIXME is this include used
#include <avr-halib/share/interruptLock.h>  //FIXME is this include used
#include <stdlib.h>                         //FIXME is this include used
#include "../common.h"                      //FIXME use correct include if used at all?
#include "armarow/armarow.h"                //FIXME are parts of ArMARoW used?
#include "armarow/debug.h"                  //FIXME are debugging features used?
#include "armarow/phy/phy.h"                //FIXME are parts of the physical layer used?

using avr_halib::drivers::Clock;
using namespace avr_halib::regmaps;

namespace armarow {
namespace mac {

    /*! \brief Don't know.*/
        enum MAC_EVALUATION_ACTIVATION_STATE {
        Enable  = 0, /*!< Don't know*/
        Disable = 1  /*!< Don't know*/
    };
    /*! \brief Clock configuration for the evaluation feature.
     *
     *  Will configure a clock that causes an interrupt every second calling a run to completion Task.
     */
    struct EvaluationClockConfig {
        typedef uint16_t TickValueType;
        typedef Frequency<1> TargetFrequency;
        typedef CPUClock TimerFrequency;
        typedef local::Timer4 Timer;
    };

    struct MACEvaluationInterface {
        /*! \brief Increases the internal byte count by a given amount (e.g. to measuere netto bandwith).*/
        void raiseByteCount(uint8_t numberOfBytes);
        /*! \brief Prints out statistical information periodically (run to completion task).*/
        void flushByteCount();
        /*! \brief Initializes the evaluation interface.*/
        void init();
    };

    template <MAC_EVALUATION_ACTIVATION_STATE>
    struct MACEvaluation;

    /*! \brief Implementation of the Medium Access Layer evaluation.
     *  \note   The Medium Access Layer evaluation is mainly used to output statistical information periodically and to
     *          meaure uptime as well as bandwith.
     */
    template <>
    struct MACEvaluation<Enable> : public MACEvaluationInterface {
        Clock<EvaluationClockConfig> evaluationClock;
        Led<Led0> led; //FIXME do we have a hardware independent way to use this feature?
        uint16_t bytesReceived;
        int uptimeSec;

        MACEvaluation() {
            init();
        }
        void init() {
            bytesReceived = 0;
            uptimeSec     = 0;
            evaluationClock.registerCallback<typeof *this, &MACEvaluation<Enable>::flushByteCount>(*this);
        }
        void raiseByteCount(uint8_t numberOfBytes) {
            bytesReceived += numberOfBytes;
        }
        void flushByteCount() {
            avr_halib::locking::GlobalIntLock lock;
            led.toggle();
            ::logging::log::emit() << "uptime:\t" << uptimeSec << " sec" << ::logging::log::endl;
            bytesReceived = 0; //FIXME should not the byte count be printed somewere, or be left out completely?
            uptimeSec++;
        }
    };

    /*! \brief Dummy implementation of the evaluation feature interface.
     *
     *  \note This empty implementation is used, if you disable the evaluation of the Medium Access Layer
     */
    template <>
    struct MACEvaluation<Disable> : public MACEvaluationInterface {
        MACEvaluation() {}
        void raiseByteCount(uint8_t numberOfBytes) {}
        void flushByteCount() {}
        void init() {}
    };
}
}
