#pragma once

#include "avr-halib/regmaps/local.h"
#include "avr-halib/share/freq.h"
#include "avr-halib/avr/clock.h"


struct ClockConfigShallowSleep{
	typedef uint16_t TickValueType;
	typedef TTEventFrequency TargetFrequency;
	typedef CPUClock TimerFrequency;
	typedef avr_halib::regmaps::local::Timer1 Timer;
	static const avr_halib::power::SleepModes sleepMode=avr_halib::power::idle;
};

struct ClockConfigDeepSleep{
	typedef uint16_t TickValueType;
	typedef TTEventFrequency TargetFrequency;
	typedef Frequency<32768> TimerFrequency;
	typedef avr_halib::regmaps::local::Timer2 Timer;
	static const avr_halib::power::SleepModes sleepMode=avr_halib::power::powerSave;
}; 

typedef ClockConfigDeepSleep ClockConfig;

UseInterrupt(SIG_OUTPUT_COMPARE2A);
