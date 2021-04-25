/*******************************************************************************
Project   Open Car Charging Station
Author: digitalmarc.ch
*******************************************************************************/

/*@attention
*
* The Poti at position 0 has the maximum resistance between the VH and VW!!
*
*/

//-----------------------------------------------------------------------------
// includes
#include "IRSender.h"



//-----------------------------------------------------------------------------
// defines

#define IR_ON 0
#define IR_OFF 1

#define FORWARD 1

//DCC
#define L 116
#define H 58
#define PREAMBLE 14

//-----------------------------------------------------------------------------
// declarations


//*****************************************************************************
// description:
//   constructor
//*****************************************************************************

IRSender::IRSender(uint8_t pin)
{
	_pin = pin;

	// Define this pin as output
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, IR_OFF);
}

IRSender::~IRSender()
{
}

void IRSender::Speed(uint8_t address, uint8_t speed)
{
	uint8_t data = 0b01000000 | (speed & 0x0F) | (FORWARD << 5);
	uint8_t crc = address ^ data;

	dccPreamble();
	zero();
	dccAddress(address);
	zero();
	dccData(data);
	zero();
	dccCrc(crc);
	stopBit();
}

void IRSender::Function(uint8_t address, uint8_t function, bool on)
{
	uint8_t data = 0;
	uint8_t crc = 0;

	if (function <= 4)
	{
		data = 0b10000000;

		if (function == 0)
		{
			data |= (1 << 4);
		}
		else
		{
			data |= (1 << (function - 1));
		}
	}
	else if (function >= 5 && function <= 8)
	{
		data = 0b10110000 | (1<< (function - 5));
	}
	else if (function >= 9 && function <= 12)
	{
		data = 0b10100000 | (1 << (function - 9));
	}

	crc = address ^ data;

	dccPreamble();
	zero();
	dccAddress(address);
	zero();
	dccData(data);
	zero();
	dccCrc(crc);
	stopBit();
}

void IRSender::DCC_Bit(uint8_t b)
{
	int duration;
	if (b)
	{
		duration = H;
	}
	else
	{
		duration = L;
	}

	digitalWrite(_pin, IR_ON);
	delayMicroseconds(duration);
	digitalWrite(_pin, IR_OFF);
	delayMicroseconds(duration);
}

void IRSender::dccPreamble()
{
	int i = 0;
	for (i = 1; i < PREAMBLE; i++)
	{
		DCC_Bit(1);
	}
}

void IRSender::zero()
{
	DCC_Bit(0);
}

void IRSender::dccAddress(uint8_t adr)
{
	for (int a = 7; a >= 0; a--)
	{
		DCC_Bit(((adr >> a) & 0x01));
	}
}
void IRSender::dccData(uint8_t data)
{
	for (int d = 7; d >= 0; d--)
	{
		DCC_Bit(((data >> d) & 0x01));
	}
}

void IRSender::dccCrc(uint8_t crc)
{
	for (int c = 7; c >= 0; c--)
	{
		DCC_Bit(((crc >> c) & 0x01));
	}
}

void IRSender::stopBit()
{
	DCC_Bit(1);
}
