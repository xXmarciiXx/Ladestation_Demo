/*******************************************************************************
Project   Open Car Charging Station
Author: digitalmarc.ch
*******************************************************************************/


//-----------------------------------------------------------------------------
// includes
#include "ChargeScreen.h"
#include <SPI.h>


//-----------------------------------------------------------------------------
// defines


//*****************************************************************************
// description:
//   constructor
//*****************************************************************************
ChargeScreen::ChargeScreen(uint8_t CS_PIN, uint8_t DC_PIN, uint8_t RST_PIN)
{
	_screen = new TFT(CS_PIN, DC_PIN, RST_PIN);

	// Put this line at the beginning of every sketch that uses the GLCD:
	_screen->begin();

	_screen->setAddrWindow(2, 27, 20, 47);
	_screen->invertDisplay(true);
	_screen->setRotation(1);

	// clear the screen with a black background
	_screen->background(0, 0, 0);

	// write the static text to the screen
	// set the font color to white
	_txtColor = _screen->newColor(255, 255, 255);
	_screen->stroke(_txtColor);
	// set the font size
	_screen->setTextSize(1);

	_sOld = 70;
}


//*****************************************************************************
// description:
//   destructor
//*****************************************************************************
ChargeScreen::~ChargeScreen()
{
	free(_screen);
}


void ChargeScreen::printCarInfo(uint8_t address, uint16_t mAh, uint8_t cRate, char* type)
{
	_printHeader("Car Info", 255, 255, 255);
	
	_clearInfoSection();
	_setTextColor(255, 255, 255);
	_screen->setTextSize(1);

	// draw Text
	char tempText[30] = {}; 
	
	// address
	memset(tempText, 0, sizeof(tempText));
	sprintf(tempText, "Address:  %d", address);
	_screen->text(tempText, 2, 50);

	// Capacity
	memset(tempText, 0, sizeof(tempText));
	sprintf(tempText, "Capacity: %dmAh", mAh);
	_screen->text(tempText, 2, 60);

	// C-Rate
	memset(tempText, 0, sizeof(tempText));
	sprintf(tempText, "C rate:   %d", cRate);
	_screen->text(tempText, 2, 70);

	// Type
	memset(tempText, 0, sizeof(tempText));
	sprintf(tempText, "Car:      %s", type);
	_screen->text(tempText, 2, 80);

}

void ChargeScreen::printNoCar()
{
	_printHeader("Standby", 0, 255, 0);

	_clearInfoSection();
	_screen->setTextSize(1);
	_setTextColor(0, 255, 0);
	_screen->text("Ready for new car", 2, 60);
}

void ChargeScreen::printChargeResistor(uint32_t resistorValue)
{
	// draw Text
	char tempText[30] = {};

	// address
	memset(tempText, 0, sizeof(tempText));
	if (resistorValue >= 1000)
	{
		sprintf(tempText, "Resistor: %d kOhm", resistorValue/1000);
	}
	else
	{
		sprintf(tempText, "Resistor: %d Ohm", resistorValue);
	}
	
	_screen->setTextSize(1);
	_setTextColor(255, 255, 255);
	_screen->text(tempText, 2, 90);
}

void ChargeScreen::printError(char* errorString)
{
	_printHeader("ERROR", 255, 0, 0);
	// draw Text
	char tempText[30] = {};

	// Type
	memset(tempText, 0, sizeof(tempText));
	sprintf(tempText, "ERROR: %s", errorString);

	_screen->setTextSize(1);
	_setTextColor(255, 0, 0);
	_screen->text(tempText, 2, 90);
}

void ChargeScreen::printLED(uint8_t num, int state)
{
	if (num == 0 && state == 1)
	{
		_screen->drawCircle(157, 90, 3, _screen->newColor(255, 0, 0));
	}
	else if (num == 0)
	{
		_screen->drawCircle(157, 90, 3, _screen->newColor(0, 0, 0));
	}

	if (num == 1 && state == 1)
	{
		_screen->drawCircle(157, 100, 3, _screen->newColor(0, 255, 0));
	}
	else if (num == 1)
	{
		_screen->drawCircle(157, 100, 3, _screen->newColor(0, 0, 0));
	}
	
}

void ChargeScreen::printCharging(uint32_t ms)
{
	char time[10] = {};
	memset(time, 0, sizeof(time));

	uint8_t h = (ms / 3600000);
	ms -= (h * 3600000);
	uint8_t m = (ms / 60000);
	ms -= (m * 60000);
	uint8_t s = (ms / 1000);

	_printHeader("Charging", 255, 0, 0);
	_clearInfoSection();
	sprintf(time, "%02d:%02d:%02d", h, m, s);
	_screen->setTextSize(2);
	_setTextColor(255, 255, 255);
	_screen->text(time, 30, 70);
}

void ChargeScreen::printChargingUpdateTime(uint32_t ms)
{
	char time[10] = {};
	memset(time, 0, sizeof(time));

	uint8_t h = (ms / 3600000);
	ms -= (h * 3600000);
	uint8_t m = (ms / 60000);
	ms -= (m * 60000);
	uint8_t s = (ms / 1000);

	if (_sOld != s)
	{
		sprintf(time, "%02d:%02d:%02d", h, m, s);
		_screen->setTextSize(2);
		_setTextColor(255, 255, 255);
		_clearTimeSection();
		_screen->text(time, 30, 70);
	}
	_sOld = s;
}

void ChargeScreen::printFinishCharging(uint32_t ms)
{
	char time[10] = {};
	memset(time, 0, sizeof(time));

	uint8_t h = (ms / 3600000);
	ms -= (h * 3600000);
	uint8_t m = (ms / 60000);
	ms -= (m * 60000);
	uint8_t s = (ms / 1000);

	_printHeader("Finished", 0, 255, 0);
	_clearInfoSection();
	sprintf(time, "%02d:%02d:%02d", h, m, s);
	_screen->setTextSize(2);
	_setTextColor(255, 255, 255);
	_screen->text(time, 30, 70);
}

void ChargeScreen::_printHeader(char* header, uint8_t r, uint8_t g, uint8_t b)
{
	_screen->fillRect(0, 0, 160, 50, _screen->newColor(0, 0, 0)); // Clear only the Header Area
	_screen->setTextSize(2);
	_setTextColor(r, g, b);
	_screen->text(header, 2, 30);
}

void ChargeScreen::_setTextColor(uint8_t r, uint8_t g, uint8_t b)
{
	_txtColor = _screen->newColor(r, g, b);
	_screen->stroke(_txtColor);
}

void ChargeScreen::_clearInfoSection()
{
	_screen->fillRect(0, 50, 160, 70, _screen->newColor(0, 0, 0)); // Clear only the Info Section
}

void ChargeScreen::_clearTimeSection()
{
	_screen->fillRect(30, 69, 95, 15, _screen->newColor(0, 0, 0)); // Clear only the Info Section
}
