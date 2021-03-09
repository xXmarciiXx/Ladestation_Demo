/*******************************************************************************
Project
================================================================================

*******************************************************************************/


#ifndef _CAR_INFO_NFC_H
#define _CAR_INFO_NFC_H


//----------------------------------------------------------------------------
// include
#include "Arduino.h"
#include <NfcAdapter.h>

//----------------------------------------------------------------------------
// enum

#define MAX_LENGTH_CARTYPE 11
#define MAX_LENGTH_BUILDDATE 11
#define MAX_LENGTH_OWNER 41

struct carInformation_t
{
	uint16_t address;
	uint16_t capacity;
	uint8_t	 chargingRate;
	uint8_t  carType[MAX_LENGTH_CARTYPE];
	uint8_t  buildDate[MAX_LENGTH_BUILDDATE];
	uint8_t  owner[MAX_LENGTH_OWNER];
};


//----------------------------------------------------------------------------
// class
class CarInfoNFC
{
public:

	CarInfoNFC();
	~CarInfoNFC();

	bool getCarPresent();
	bool getCarInfo(carInformation_t* carInfo);

private:
	
	void _decodeCarInfo(NdefMessage* ndefMessage, carInformation_t* carInfo);
};

#endif

