#pragma once

//ArMARoW includes
#include <armarow/armarow.h>
#include <armarow/debug.h>
//HALIB includes
#include <avr-halib/avr/clock.h>
#include <avr-halib/avr/regmaps.h>
#include <avr-halib/avr/timer.h>
#include <avr-halib/regmaps/local.h>
#include <avr-halib/share/delay.h>
#include <avr-halib/share/delegate.h>
#include <avr-halib/share/interruptLock.h>
//AVR includes
#include <stdlib.h>
#include "../common.h"


namespace armarow {
namespace evaluation {

    /*! \brief This is the NoEvaluator and by the way defines it the Interface of the Evaluator class.*/
    struct NoEvaluator
    {
        template<typname T> push(T value){};//NoEvaluator takes all types - not every Evaluator may do;
              //called by: observed         //push should be aware of beeing called while stoped
        void init(){};  // observed or app  //basic setup should at least contain reset and start
                                            //if in doubt of status call this to start
        void start(){}; // app              //if evaluation is stopable due performance reasons this starts it again
        void stop(){};  // app              //if evaluation is stopable this stops it (eg.:may switch off callbacks)
        void reset(){}; // see init         //call this to reset the evaluation (may reset counter if running dont stop)
        void report(){};// app              //give report now 
                                            //(if periodic report is given this should not start a new period)
        //every method may be without Funktion but implementd (you may simply inherit form this)
        //most working Evaluators will need at least a push and a init funktion
        //init should be called by the observed object and it should report to push
        //if the observed object is reseted it should call reset
        //an internal (eg Timer) triggert report or working report method
        //reports may be given any time there may be more channels than the report method
    }

    /*! \brief This is the actual implementation of the evaluation feature for the mac protocol.
     may be passed to the Mac_Layer. This class is mainly used to output statistical information 
     every second and is used to meaure uptime and bandwith.*/
    struct PeriodicRateEvaluator: public NoEvaluator
    {
        using     avr_halib::drivers::Clock;
        namespace regmaps = avr_halib::regmaps;
        namespace log = ::logging::log;

        struct Config
        {
            /*! \brief This is the configuration class for the Clock needed for the evaluation feature. 
             The clock will cause an interrupt every Second and call a run to completion Task.*/
            struct EvaluationClockConfig {
                typedef uint16_t TickValueType;
                typedef Frequency<1> TargetFrequency;
                typedef CPUClock TimerFrequency;
                typedef regmaps::local::Timer4 Timer;
            };
            typedef avr_halib::locking::GlobalIntLock Guard;
            typedef Clock<EvaluationClockConfig> Periodic;
        }

        Config::Periodic clock;
        uint16_t accu;
        uint8_t period;

        void init() {
            period = 0;
            accu = 0;
            clock.registerCallback<typeof *this, &PeriodicRateEvaluator::periodreport>(*this);
            this->uptime_in_sec = 0;
        }

        void push(uint8_t value) {
            config::Guard lock;
            accu += value;
        }

        void periodreport() {
            config::Guard lock;
            log::emit() << "period:\t" << period << "last value:\t"<< accu << log::endl;
            accu = 0;
            period++;
        }

        void report() {
            config::Guard lock;
            log::emit() << "period:\t" << period << "courrent value:"<< accu << log::endl;
        }
    };
}
}
