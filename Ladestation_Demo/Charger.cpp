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
#include "Charger.h"



//-----------------------------------------------------------------------------
// defines

//-----------------------------------------------------------------------------
// declarations


//*****************************************************************************
// description:
//   constructor
//*****************************************************************************
Charger::Charger(uint32_t maxResistance, uint8_t pinCS, uint8_t pinUD, uint8_t pinINC)
{
	_resistanceStep = maxResistance / X9_STEPS;
	_maxResistance = maxResistance;

	_pinCS = pinCS;
	_pinUD = pinUD;
	_pinINC = pinINC;

	// Setup GPIOs
	pinMode(_pinCS, OUTPUT);
	pinMode(_pinUD, OUTPUT);
	pinMode(_pinINC, OUTPUT);

	digitalWrite(_pinCS, LOW);
	digitalWrite(_pinUD, LOW);
	digitalWrite(_pinINC, HIGH);

	Reset();
}

//*****************************************************************************
// description:
//   destructor
//*****************************************************************************
Charger::~Charger()
{

}

uint32_t Charger::GetEstimatedResistance()
{
	return _currentStep * _resistanceStep;
}

// The reset does drives the wiper to the lowest position (0) and this results to the max resistor value, not 0!!!
void Charger::Reset()
{
	digitalWrite(_pinCS, LOW);
	digitalWrite(_pinUD, LOW);
	digitalWrite(_pinINC, HIGH);

	for (uint8_t i = 0; i < X9_STEPS; i++)
	{
		digitalWrite(_pinINC, LOW);
		delayMicroseconds(NINC_HIGH_PERIOD);
		digitalWrite(_pinINC, HIGH);
		delayMicroseconds(NINC_LOW_PERIOD);
	}
	digitalWrite(_pinCS, HIGH);
	digitalWrite(_pinUD, HIGH);

	_currentStep = 0;
}

bool Charger::SetResistor(uint32_t current_mA)
{
	// return value
	bool success = false;

	// Poti steps
	uint16_t steps;

	// get the resistor value in Ohm based on the formula for the charge current of the TC4056 
	uint32_t resistor = (uint32_t) (((double)1 / (((double)current_mA / (double)1200))) * 1000); // *1000 because we give mA as input and we receive kOhm as output but we like Ohm

	// For security reasons, always reset the Poti!!
	Reset();

	if (resistor > _maxResistance)
	{
		// not good, we can not reach the resistor value!!
		success = false;
	}
	else
	{
		steps = (resistor / _resistanceStep);

		// now step upwards
		for (uint8_t i = 0; i < steps; i++)
		{
			_up();
		}

		success = true;
	}


	return success;
}

void Charger::_down()
{
	digitalWrite(_pinINC, HIGH);
	digitalWrite(_pinCS, LOW);
	digitalWrite(_pinUD, LOW);

	delayMicroseconds(NINC_HIGH_TO_UND_CHANGE);
	digitalWrite(_pinINC, LOW);

	if (_currentStep > 0)
	{
		_currentStep--;
	}
}

void Charger::_up()
{
	digitalWrite(_pinINC, HIGH);
	digitalWrite(_pinCS, LOW);
	digitalWrite(_pinUD, HIGH);

	delayMicroseconds(NINC_HIGH_TO_UND_CHANGE);
	digitalWrite(_pinINC, LOW);

	if (_currentStep < X9_STEPS)
	{
		_currentStep++;
	}
}




