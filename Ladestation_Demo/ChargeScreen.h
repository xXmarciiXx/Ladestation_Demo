/*******************************************************************************
Project   
================================================================================

*******************************************************************************/


#ifndef _CHARGE_SCREEN_H
#define _CHARGE_SCREEN_H


//----------------------------------------------------------------------------
// include
#include "Arduino.h"
#include <TFT.h>  // Arduino LCD library


//----------------------------------------------------------------------------
// enum




//----------------------------------------------------------------------------
// class
class ChargeScreen
{
public:

	ChargeScreen(uint8_t CS_PIN, uint8_t DC_PIN, uint8_t RST_PIN);
	~ChargeScreen();

	void printCarInfo(uint8_t address, uint16_t mAh, uint8_t cRate, char* type);
	void printNoCar();
	void printChargeResistor(uint32_t resistorValue);
	void printError(char* errorString);
	void printLED(uint8_t num, int state);
	void printCharging(uint32_t ms);
	void printChargingUpdateTime(uint32_t ms);
	void printFinishCharging(uint32_t ms);

private:
	const uint8_t VIEWING_AREA_WIDTH = 160;
	const uint8_t VIEWING_AREA_HEIGHT = 80;
	const uint8_t CHAR_WIDTH = 5;
	const uint8_t CHAR_SPACE_WIDTH = 1;

	const uint8_t X_POS_START = 1;
	const uint8_t X_POS_TYPE = 3;
	const uint8_t X_POS_TIME = 27;
	const uint8_t X_POS_TARGET = 65;
	const uint8_t X_POS_PLATFORM = 154;
	const uint8_t Y_POS_START = 27;
	const uint8_t LINE_HEIGHT = 10;
	const uint8_t TYPE_BG_WIDTH = (3 * (CHAR_WIDTH + CHAR_SPACE_WIDTH)) + 2;
	const uint8_t TYPE_BG_HEIGHT = 9;


	TFT* _screen;
	uint16_t _txtColor;

	uint8_t _sOld;

	void _setTextColor(uint8_t r, uint8_t g, uint8_t b);
	void _printHeader(char* header, uint8_t r, uint8_t g, uint8_t b);
	void _clearInfoSection();
	void _clearTimeSection();
};

#endif
