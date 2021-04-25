/*******************************************************************************
Project
================================================================================

 Note the GPIO pin is active LOW!

*******************************************************************************/


#ifndef _IRSENDER_H
#define _IRSENDER_H

// include
#include "Arduino.h"

class IRSender
{
	public:
		IRSender(uint8_t pin);
		~IRSender();

		void Speed(uint8_t address, uint8_t speed);
		void Function(uint8_t address, uint8_t function, bool on);


	private:
		uint8_t _pin;

		void DCC_Bit(uint8_t b);
		void dccPreamble();
		void zero();
		void dccAddress(uint8_t adr);
		void dccData(uint8_t data);
		void dccCrc(uint8_t crc);
		void stopBit();
};

#endif
