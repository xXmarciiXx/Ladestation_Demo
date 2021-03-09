/*******************************************************************************
Project
================================================================================

*******************************************************************************/


#ifndef _CHARGER_H
#define _CHARGER_H

//In microseconds.
#define NCS_TO_NINC_SETUP				1
#define NINC_HIGH_TO_UND_CHANGE			1
#define UND_TO_NINC_SETUP				3
#define NINC_LOW_PERIOD					1
#define NINC_HIGH_PERIOD				1
#define NINC_INACTIVE_TO_NCS_INACTIVE	1
#define NCS_DESELECT_TIME_STORE			20000
#define NCS_DESELECT_TIME_NO_STORE		1
#define NINC_TO_VWRW_CHANGE				100
#define NINC_CYCLE_TIME					2
#define POWER_UP_TO_WIPER_STABLE		500

#define X9_STEPS 100

// X9 Types
#define X9C102_RESISTANCE 1000 // X9C102 = 1kOhm
#define X9C103_RESISTANCE 10000 // X9C103 = 10kOhm
#define X9C503_RESISTANCE 50000 // X9C503 = 50kOhm
#define X9C104_RESISTANCE 100000 // X9C104 = 100kOhm


//----------------------------------------------------------------------------
// include
#include "Arduino.h"

//----------------------------------------------------------------------------
// enum


//----------------------------------------------------------------------------
// class
class Charger
{
public:

	Charger(uint32_t maxResistance, uint8_t pinCS, uint8_t pinUD, uint8_t pinINC);
	~Charger();
	uint32_t GetEstimatedResistance();
	void Reset();
	bool SetResistor(uint32_t current_mA);

private:

	uint8_t _pinCS;
	uint8_t _pinUD;
	uint8_t _pinINC;

	uint32_t _maxResistance;
	uint8_t _currentStep;
	uint32_t _resistanceStep;

	void _down();
	void _up();

};

#endif

