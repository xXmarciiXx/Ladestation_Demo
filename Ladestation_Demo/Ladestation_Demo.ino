/*
 Name:		Ladestation_Demo.ino
 Created:	03.01.2021 21:07:14
 Author:	Marc
*/

#include "ChargeScreen.h"
#include "CarInfoNFC.h"

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

CarInfoNFC* carInfo;

uint8_t state = 0;


// Resistor Charge current
#include "Charger.h"
#define CR_CS_PIN 3
#define CR_UD_PIN 7
#define CR_INC_PIN 5
#define CHRG0_PIN A1 // Charging
#define CHRG1_PIN A2 // STBY

Charger* charger;

// Servo
#include "Servo.h"
#define SERVO_IDLE_POS 90
#define SERVO_MAX_POS 110

Servo servo;

uint32_t _startTime = 0;

// the setup function runs once when you press reset or power the board
void setup() 
{
	pinMode(CHRG0_PIN, INPUT_PULLUP);
	pinMode(CHRG1_PIN, INPUT_PULLUP);

	carInfo = new CarInfoNFC();
	chargeScreen = new ChargeScreen(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
	
	chargeScreen->printNoCar();

	charger = new Charger(X9C503_RESISTANCE, CR_CS_PIN, CR_UD_PIN, CR_INC_PIN);

	state = 0;

	servo.attach(6);
	servo.write(SERVO_IDLE_POS);
}

// the loop function runs over and over again until power down or reset

// @attention if the car is lift up, it looses the NFC Tag!!
void loop() 
{
	if (carInfo->getCarPresent())
	{
		if (state == 0)
		{
			carInformation_t carData = {};
			if (carInfo->getCarInfo(&carData))
			{
				chargeScreen->printCarInfo(carData.address, carData.capacity, carData.chargingRate, (char*)carData.carType);
				if (charger->SetResistor(carData.capacity) == true)
				{
					chargeScreen->printChargeResistor(charger->GetEstimatedResistance());

					servoUp();
					state = 1;
				}
				else
				{
					chargeScreen->printError("Resistor value");
				}
			}
			else
			{
				chargeScreen->printError("Empty values");
				state = 0;
			}
		}
		if (state == 1)
		{
			if (digitalRead(CHRG0_PIN) == 1 && digitalRead(CHRG1_PIN) == 0)
			{
				state = 2;
				_startTime = millis();
				chargeScreen->printCharging(millis() - _startTime);
			}
		}
		if (state == 2)
		{
			chargeScreen->printChargingUpdateTime(millis()-_startTime);

			if (digitalRead(CHRG0_PIN) == 0 && digitalRead(CHRG1_PIN) == 1)
			{
				state = 3;
				chargeScreen->printFinishCharging(millis() - _startTime);
				servoIdle();
			}
		}
	}
	else
	{
		if (state != 0)
		{ 
			state = 0;
			chargeScreen->printNoCar();
			//servoIdle();
		}
	}

	chargeScreen->printLED(0, digitalRead(CHRG0_PIN));
	chargeScreen->printLED(1, digitalRead(CHRG1_PIN));
	
	delay(1);
}

void servoUp()
{
	for (int i = SERVO_IDLE_POS; i < SERVO_MAX_POS; i++)
	{
		servo.write(i);
		delay(50);
	}
}

void servoIdle()
{
	for (int i = SERVO_MAX_POS; i > SERVO_IDLE_POS; i--)
	{
		servo.write(i);
		delay(50);
	}
}