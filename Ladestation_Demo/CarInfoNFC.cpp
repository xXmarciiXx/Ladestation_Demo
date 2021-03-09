/*******************************************************************************
Project   Open Car Charging Station
Author: digitalmarc.ch
*******************************************************************************/


//-----------------------------------------------------------------------------
// includes
#include "CarInfoNFC.h"
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>


//-----------------------------------------------------------------------------
// defines

PN532_I2C pn532_i2c(Wire);
NfcAdapter _nfcAdapter = NfcAdapter(pn532_i2c);

//*****************************************************************************
// description:
//   constructor
//*****************************************************************************
CarInfoNFC::CarInfoNFC()
{
	_nfcAdapter.begin();
}


//*****************************************************************************
// description:
//   destructor
//*****************************************************************************
CarInfoNFC::~CarInfoNFC()
{
	
}

bool CarInfoNFC::getCarPresent()
{
	return _nfcAdapter.tagPresent();
}

bool CarInfoNFC::getCarInfo(carInformation_t* carInfo)
{
	bool success = false;
	NfcTag tag = _nfcAdapter.read();

	NdefMessage message = tag.getNdefMessage();

	
	_decodeCarInfo(&message, carInfo);

	/* check for data integrity */
	if (carInfo->address == 0 || carInfo->capacity == 0 || carInfo->chargingRate == 0)
	{
		success = false;
	}
	else
	{
		success = true;
	}

	return success;
}

void CarInfoNFC::_decodeCarInfo(NdefMessage* ndefMessage, carInformation_t* carInfo)
{
	uint8_t count = ndefMessage->getRecordCount();
	uint8_t buffer[50] = { 0 };

	// Index 0 - The Address
	if (count >= 1)
	{
		ndefMessage->getRecord(0).getPayload(buffer);
		carInfo->address = atoi((char*)&buffer[3]);
	}

	// Index 1 - The Capacity
	if (count >= 2)
	{
		ndefMessage->getRecord(1).getPayload(buffer);
		carInfo->capacity = atoi((char*)&buffer[3]);
	}

	// Index 2 - The Capacity
	if (count >= 3)
	{
		ndefMessage->getRecord(2).getPayload(buffer);
		carInfo->chargingRate = atoi((char*)&buffer[3]);
	}

	// Index 3 - Type
	if (count >= 4)
	{
		ndefMessage->getRecord(3).getPayload(buffer);
		uint8_t len = ndefMessage->getRecord(3).getPayloadLength() - 3;

		if (len > MAX_LENGTH_CARTYPE)
		{
			len = MAX_LENGTH_CARTYPE;
		}
		memcpy(&carInfo->carType, &buffer[3], len);
	}

	// Index 4 - build date
	if (count >= 4)
	{
		ndefMessage->getRecord(4).getPayload(buffer);
		uint8_t len = ndefMessage->getRecord(4).getPayloadLength() - 3;

		if (len > MAX_LENGTH_BUILDDATE)
		{
			len = MAX_LENGTH_BUILDDATE;
		}
		memcpy(&carInfo->buildDate, &buffer[3], len);
	}

	// Index 5 - owner
	if (count >= 5)
	{
		ndefMessage->getRecord(5).getPayload(buffer);
		uint8_t len = ndefMessage->getRecord(5).getPayloadLength() - 3;

		if (len > MAX_LENGTH_OWNER)
		{
			len = MAX_LENGTH_OWNER;
		}
		memcpy(&carInfo->owner, &buffer[3], len);
	}
}
