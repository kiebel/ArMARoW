

#pragma once
#include "avr-halib/avr/interrupt.h"
#include <stdint.h>


/**
 *	\brief		Register map for Timer3 of atmega1281
 *	\ingroup	atmega1281
 *
 *	\todo		Add input capture support
 */
class Timer3
{
public:
	/// Counter register width
	enum { counterWidth = 16 };
	
	/// Clock sources of Timer3
	enum ClockSelect {
		stop = 0,	///< no clock source
		ps1 = 1,	///< internal clock
		ps8 = 2,	///< internal clock, prescaler 8
		ps64 = 3,	///< internal clock, prescaler 64
		ps256 = 4,	///< internal clock, prescaler 256
		ps1024 = 5,	///< internal clock, prescaler 1024
		extFalling = 6,	///< external clock on falling edge
		extRising = 7	///< external clock on rising edge
	};
	void setCS(ClockSelect cs)
	{
		 _clockSelect = cs;
	}
	
	// TODO: add other supported modes  
	//see documentation atmega 1281 page 160
	/// Waveform generation modes of Timer3
	enum WaveformGenerationMode {
		normal = 0,		///< normal mode
		phaseCorrectPwm = 1,	///< phase correct pwm, 8 bit
		phaseCorrectPwm9 = 2,	///< phase correct pwm, 9 bit
		phaseCorrectPwm10 = 3,	///< phase correct pwm, 10 bit
		ctc = 4,		///< clear timer on compare match
		fastPwm = 5,		///< fast pwm, 8 bit
		fastPwm9 = 6,		///< fast pwm, 9 bit
		fastPwm10 = 7,		///< fast pwm, 10 bit
				
		phaseFrequencyCorrectPwmICRn = 8, ///<pwm phase and frequency correct
		phaseFrequencyCorrectPwmOCRnA = 9, ///<pwm phase and frequency correct
		phaseCorrectPwmICRn = 10,  ///<pwm phase correct
		phaseCorrectPwmOCRnA = 11, ///<pwm phase correct
		ctcICRn = 12, ///<CRC
		reserved = 13, ///<reserved
		fastPwmICRn = 14, ///<fast pwm
		fastPwmOCRnA = 15 ///<fast pwm
		
	};
	void setWGM(WaveformGenerationMode i)
	{
		_waveformGenerationMode01 = i;
		_waveformGenerationMode23 = i >> 2;
	}
	WaveformGenerationMode getWGM()
	{
		return (WaveformGenerationMode)((_waveformGenerationMode23 << 2) | _waveformGenerationMode01);
	}

	/// Compare match output mode (controls OC3A, OC3B, OC3C on compare match)
	enum CompareMatchOutputMode
	{
		disconnected = 0,	///< no change
		toggle = 1,		///< toggle on compare match
		clear = 2,		///< clear on compare match
		set = 3			///< set on compare match
	};
	void setCOMA(CompareMatchOutputMode com)
	{
		_compareMatchOutputModeA = com;
	}
	void setCOMB(CompareMatchOutputMode com)
	{
		_compareMatchOutputModeB = com;
	}
	void setCOMC(CompareMatchOutputMode com)
	{
		_compareMatchOutputModeC = com;
	}
	

	template<class T, void (T::*Fxn)()>
	static void setOutputCompareAInterrupt(T & obj)
	{
		redirectISRM(TIMER3_COMPA_vect, Fxn, obj);
	}
	
	template<class T, void (T::*Fxn)()>
	static void setOutputCompareBInterrupt(T & obj)
	{
		redirectISRM(TIMER3_COMPB_vect, Fxn, obj);
	}
	
	template<class T, void (T::*Fxn)()>
	static void setOutputCompareCInterrupt(T & obj)
	{
		redirectISRM(TIMER3_COMPC_vect, Fxn, obj);
	}

	template<class T, void (T::*Fxn)()>
	static void setOverflowInterrupt(T & obj)
	{
		redirectISRM(TIMER3_OVF_vect, Fxn, obj);
	}

	template<class T, void (T::*Fxn)()>
	static void setInputCaptureInterrupt(T & obj)
	{
		redirectISRM(TIMER3_CAPT_vect, Fxn, obj);
	}

private:
	uint8_t __base [0x71];

public:
// TIMSK3 (0x71) {
	/// Interrupt mask bits
	enum
	{
		im_disable = 0,			///< Disable Timer1's interrupts
		im_overflowEnable = 1,		///< Enable \c TIMER3_CAPT_vect
		im_outputCompareAEnable = 2,	///< Enable \c TIMER3_COMPA_vect
		im_outputCompareBEnable = 4,	///< Enable \c TIMER3_COMPB_vect
		im_outputCompareCEnable = 8,	///< Enable \c TIMER3_COMPC_vect
		im_inputCaptureEnable = 32	///< Enable \c TIMER3_OVF_vect
	};
	/// Interrupt mask (enables/disables Timer3's interrupts)
	uint8_t interruptMask : 6;

	bool : 2;
// }

private:	
	uint8_t __pad0 [0x90 - 0x71 - 1];

// TCCR3A (0x90) {
	uint8_t _waveformGenerationMode01 : 2;
	uint8_t _compareMatchOutputModeC : 2;
	uint8_t _compareMatchOutputModeB : 2;
	uint8_t _compareMatchOutputModeA : 2;
// }

// TCCR3B (0x91) {
	uint8_t _clockSelect : 3;

	uint8_t _waveformGenerationMode23 : 2;
	uint8_t : 3;
// }
	
private:	
// TCCR3C (0x92)
// TCNT3L (0x94)
// TCNT3H (0x95)
// ICR3L (0x96)
// ICR3H (0x97)
	uint8_t __pad1 [0x98 - 0x91 - 1];

public:	
// OCR3AL (0x98) {
	/// Output compare register A (low byte)
	uint8_t outputCompareA : 8;
// OCR3AH (0x99) {
	/// Output compare register A (high byte)
	uint8_t outputCompareAH : 8;
// OCR3BL (0x9a) {
	/// Output compare register B (low byte)
	uint8_t outputCompareB : 8;
// OCR3BH (0x9b) {
	/// Output compare register B (high byte)
	uint8_t outputCompareBH : 8;
// OCR3CL (0x9c) {
	/// Output compare register C (low byte)
	uint8_t outputCompareC : 8;
// OCR3CH (0x9d) {
	/// Output compare register C (high byte)
	uint8_t outputCompareCH : 8;
}__attribute__((packed));


