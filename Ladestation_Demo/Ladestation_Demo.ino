/*
 Name:		Ladestation_Demo.ino
 Created:	03.01.2021 21:07:14
 Author:	Marc
*/

#include "ChargeScreen.h"
#include "CarInfoNFC.h"
#include "IRSender.h"

// TFT Screen
#include <SPI.h>
#include <TFT.h>
// pin definition for the Uno / Nano
#define TFT_CS_PIN   10
#define TFT_DC_PIN   9
#define TFT_RST_PIN  8
#define TFT_SCL_PIN  13
#define TFT_MOSI_PIN 11

ChargeScreen* chargeScreen;

// NFC Info
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

typedef enum EState
{
	eSTATE_STANDBY = 0,
	eSTATE_PREPARE = 1,
	eSTATE_CHARGE = 2,
	eSTATE_FINISHED = 3,
};

CarInfoNFC* carInfo;

EState state = eSTATE_STANDBY;


// Resistor Charge current
#include "Charger.h"
#define CR_CS_PIN 3
#define CR_UD_PIN 7
#define CR_INC_PIN 5
#define CHRG0_PIN A1 // Charging
#define CHRG1_PIN A2 // STBY

Charger* charger;

// Servo
#define SERVO_DOWN 0
#define SERVO_UP 1
#define SERVO_PIN 6

// IR Sender
IRSender* irSender;
#define IR_OUT_PIN A3

// IR Stop
#define IR_STOP_PIN 4

uint32_t _startTime = 0;

// Variables
bool carPresent = false;
carInformation_t carData;

// the setup function runs once when you press reset or power the board
void setup()
{
	pinMode(CHRG0_PIN, INPUT_PULLUP);
	pinMode(CHRG1_PIN, INPUT_PULLUP);
	pinMode(SERVO_PIN, OUTPUT);
	digitalWrite(SERVO_PIN, SERVO_DOWN);

	pinMode(IR_STOP_PIN, OUTPUT);

	carInfo = new CarInfoNFC();
	chargeScreen = new ChargeScreen(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

	chargeScreen->printNoCar();

	charger = new Charger(X9C503_RESISTANCE, CR_CS_PIN, CR_UD_PIN, CR_INC_PIN);

	irSender = new IRSender(IR_OUT_PIN);

	state = eSTATE_STANDBY;
}

// @attention if the car is lift up, it looses the NFC Tag!!
void loop()
{
	// check for car
  if(state != eSTATE_CHARGE)
  {
    carPresent = carInfo->getCarPresent();
  }

	switch (state)
	{
	case eSTATE_STANDBY:
	{
		servoIdle();

		activateIrStop(true);

		if (carPresent)
		{
			// Read Car Info
			if (carInfo->getCarInfo(&carData))
			{
				chargeScreen->printCarInfo(carData.address, carData.capacity, carData.chargingRate, (char*)carData.carType);
				if (charger->SetResistor(carData.capacity) == true)
				{
					chargeScreen->printChargeResistor(charger->GetEstimatedResistance());

					state = eSTATE_PREPARE;
				}
				else
				{
					chargeScreen->printError("Resistor value");
				}
			}
			else
			{
				chargeScreen->printError("Empty values");
			}
		}
		break;
	}

	case eSTATE_PREPARE:
	{
		if (carPresent)
		{
			// Send DCC Command Speed 0
			irSender->Speed(carData.address, 0);
			delay(100);

			// Deactivate IR Stop (cars speed is now 0)
			activateIrStop(false);
			delay(100);

			// Switch off car
			irSender->Function(carData.address, 8, true);
			delay(100);
			irSender->Function(carData.address, 8, false);
			delay(100);

			servoUp();
      delay(1000);

			//if (digitalRead(CHRG0_PIN) == 1 && digitalRead(CHRG1_PIN) == 0)
			//{
			state = eSTATE_CHARGE;
			_startTime = millis();
			chargeScreen->printCharging(millis() - _startTime);
			//}
			//else
			//{
				// TODO
			//}
		}
		else
		{ // Car is no longer present
			state = eSTATE_STANDBY;
			chargeScreen->printNoCar();
		}
		break;
	}

	case eSTATE_CHARGE:
	{
		chargeScreen->printChargingUpdateTime(millis() - _startTime);

		if ((digitalRead(CHRG0_PIN) == 0 && digitalRead(CHRG1_PIN) == 1) || (millis()>20000))
		{
			state = eSTATE_FINISHED;
			chargeScreen->printFinishCharging(3246734);
			servoIdle();
      delay(3000);
		}
		else if (digitalRead(CHRG0_PIN) == 1 && digitalRead(CHRG1_PIN) == 1)
		{
			// Error while charging
			//state = eSTATE_FINISHED;
			//chargeScreen->printFinishCharging(millis() - _startTime);
			//servoIdle();
		}
		else
		{
			// Still charging
		}
		break;
	}

	case eSTATE_FINISHED:
	{
		if (carPresent)
		{
			// This now enables the CAR
			activateIrStop(true);
			delay(1000);

			// Drive away
			irSender->Speed(carData.address, 10);
			activateIrStop(false);
			delay(1000);
			state = eSTATE_STANDBY;
			chargeScreen->printNoCar();
		}
		else
		{
			state = eSTATE_STANDBY;
			chargeScreen->printNoCar();
		}
		break;
	}

	default:
		state = eSTATE_STANDBY;
		chargeScreen->printNoCar();
		break;
	}

	chargeScreen->printLED(0, digitalRead(CHRG0_PIN));
	chargeScreen->printLED(1, digitalRead(CHRG1_PIN));

	delay(1);
}

void servoUp()
{
	digitalWrite(SERVO_PIN, SERVO_UP);
}

void servoIdle()
{
	digitalWrite(SERVO_PIN, SERVO_DOWN);
}

void activateIrStop(bool activate)
{
	if (activate)
	{
		digitalWrite(IR_STOP_PIN, 0);
	}
	else
	{
		digitalWrite(IR_STOP_PIN, 1);
	}
}
