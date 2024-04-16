/*
 * Card_Program.c
 *
 *  Created on: 3 April 2024
 *
 *      Authors: Eng.Selim Muhammed Abbas
 *               Eng.Engy Muhammed Shehata
 */
#include <stdio.h>
#include <string.h>
#include "STD_Types.h"
#include "card.h"

#define NULL  (void*) 0

#define CARD_HOLDER_NAME_MAX_SIZE  (uint8_t) 24
#define CARD_HOLDER_NAME_MIN_SIZE  (uint8_t) 20

#define CARD_EXPIRY_DATE_MAX_SIZE  (uint8_t) 5

#define PRIMARY_ACC_NUM_MAX_SIZE  (uint8_t) 19
#define PRIMARY_ACC_NUM_MIN_SIZE  (uint8_t) 16

EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	if (cardData != NULL)
	{
		
		// Ensure null terminator is added
		cardData->cardHolderName[CARD_HOLDER_NAME_MAX_SIZE + 1] = '\0';

		uint8_t cardHolderName_length = strlen(cardData->cardHolderName);

		if (cardHolderName_length > CARD_HOLDER_NAME_MAX_SIZE || cardHolderName_length < CARD_HOLDER_NAME_MIN_SIZE)
		{
			return WRONG_NAME;
		}
		else
		{
			return CARD_OK;
		}
	}
	else
	{
		return -1;
	}

}


EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	if (cardData != NULL)
	{
		// Ensure null terminator is added
		cardData->cardExpirationDate[CARD_EXPIRY_DATE_MAX_SIZE + 1] = '\0';

		uint8_t cardExpirationDate_length = strlen(cardData->cardExpirationDate);

		if (cardExpirationDate_length > CARD_EXPIRY_DATE_MAX_SIZE || cardExpirationDate_length < CARD_EXPIRY_DATE_MAX_SIZE)
		{
			return WRONG_EXP_DATE;
		}
		else
		{
			return CARD_OK;
		}
	}
	else
	{
		return -1;
	}
}




EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	if (cardData != NULL)
	{
		
		// Ensure null terminator is added
		cardData->primaryAccountNumber[PRIMARY_ACC_NUM_MAX_SIZE + 1] = '\0';

		uint8_t primaryAccountNumber_length = strlen(cardData->primaryAccountNumber);

		if (primaryAccountNumber_length > PRIMARY_ACC_NUM_MAX_SIZE || primaryAccountNumber_length < PRIMARY_ACC_NUM_MIN_SIZE)
		{
			return WRONG_PAN;
		}
		else
		{
			return CARD_OK;
		}
	}
	else
	{
		return -1;
	}
}

