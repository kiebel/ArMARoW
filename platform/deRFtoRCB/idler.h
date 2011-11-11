#pragma once

#include <avr-halib/avr/sleep.h>

struct Idler{
	static void idle(){
		while(true)
			Morpheus::sleep<ClockConfig::sleepMode>();
	}
};
