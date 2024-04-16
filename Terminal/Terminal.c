/*
 * Terminal_Program.c
 *
 *  Created on: 3 April 2024
 *
 *      Authors: Eng.Selim Muhammed Abbas
 *               Eng.Engy Muhammed Shehata
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "STD_Types.h"
#include "card.h"
#include "terminal.h"

#define TRANS_DATE_MAX_SIZE  (uint8_t) 10
#define CARD_EXPIRY_DATE_MAX_SIZE  (uint8_t) 5

static bool isLuhnNumber(const uint16_t* primaryAccountNumber)
{
	uint16_t length_primaryAccountNumber = strlen(primaryAccountNumber);
	uint16_t checksum = 0;
	bool is_second = false;
	// Iterate over the PAN digits from right to left
	for (uint8_t local_u8Counter = length_primaryAccountNumber - 1; local_u8Counter >= 0; local_u8Counter--)
	{
		if (!isdigit(primaryAccountNumber[local_u8Counter]))
		{
			return false; // If any character is not a digit, it's an invalid PAN
		}
		// Convert char to int (by subtructing the assci of zero ('0'))
		uint16_t digit_primaryAccountNumber = primaryAccountNumber[local_u8Counter] - '0';

		// Double every second digit
		if (is_second) {
			digit_primaryAccountNumber *= 2;
			if (digit_primaryAccountNumber > 9) {
				digit_primaryAccountNumber -= 9;
			}
		}
		checksum += digit_primaryAccountNumber;
		is_second = !is_second;

	}
	// If the checksum is divisible by 10, it's a Luhn number
	return (checksum % 10 == 0);
}

EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	if (termData != NULL)
	{
		
		// Ensure null terminator is added
		termData->transactionDate[TRANS_DATE_MAX_SIZE + 1] = '\0';

		uint16_t transactionDate_length = strlen(termData->transactionDate);

		if (transactionDate_length > TRANS_DATE_MAX_SIZE || transactionDate_length < TRANS_DATE_MAX_SIZE)
		{
			return WRONG_DATE;
		}
		else
		{
			return TERMINAL_OK;
		}
	}
	else
	{
		return -1;
	}
}
EN_terminalError_t isCardExpired(ST_cardData_t* cardData, ST_terminalData_t* termData)
{
	// convert (YY) in the Date from char to digit
	uint16_t expirationDateYear = (((cardData->cardExpirationDate[CARD_EXPIRY_DATE_MAX_SIZE - 2]) - '0') * 10) + ((cardData->cardExpirationDate[CARD_EXPIRY_DATE_MAX_SIZE - 1]) - '0');
	uint16_t transactionDateYear = (((termData->transactionDate[TRANS_DATE_MAX_SIZE - 2]) - '0') * 10) + ((termData->transactionDate[TRANS_DATE_MAX_SIZE - 1]) - '0');

	// convert (MM) in the Date from char to digit
	uint16_t expirationDateMonth = (((cardData->cardExpirationDate[CARD_EXPIRY_DATE_MAX_SIZE - 5]) - '0') * 10) + ((cardData->cardExpirationDate[CARD_EXPIRY_DATE_MAX_SIZE - 4]) - '0');
	uint16_t transactionDateMonth = (((termData->transactionDate[TRANS_DATE_MAX_SIZE - 7]) - '0') * 10) + ((termData->transactionDate[TRANS_DATE_MAX_SIZE - 6]) - '0');

	// Check the card expiration by year
	if(expirationDateYear > transactionDateYear) 
	{ 
		return TERMINAL_OK;
	}

	// Check the card expiration by month
	else if (expirationDateYear == transactionDateYear )
	{
		if (expirationDateMonth >= transactionDateMonth)
		{
			return TERMINAL_OK;
		}
		else
		{
			return EXPIRED_CARD;	
		}
	}
	else
	{
		return EXPIRED_CARD;
	}

}
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	if (termData != NULL)
	{
		
		if (termData->transAmount <= 0)
		{
			return INVALID_AMOUNT;
		}
		else
		{
			return TERMINAL_OK;
		}
	}
	else
	{
		return -1;
	}
}
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	if (termData->transAmount > termData->maxTransAmount)
	{
		return EXCEED_MAX_AMOUNT;
	}
	else
	{
		return TERMINAL_OK;
	}
}
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData, f32 floatMaxAmount)
{
	if (floatMaxAmount <= 0)
	{
		return INVALID_MAX_AMOUNT;
	}
	else
	{
		termData->maxTransAmount = floatMaxAmount;
		return TERMINAL_OK;
	}
}
EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{
	if (cardData != NULL)
	{
		// Remove spaces from PAN
		cardData->primaryAccountNumber_withoutSpaces[strlen(cardData->primaryAccountNumber)];
		uint16_t local_noSpaceCounter = 0;
		for (uint16_t local_loopCounter = 0; cardData->primaryAccountNumber[local_loopCounter]; local_loopCounter++) 
		{
			if (cardData->primaryAccountNumber[local_loopCounter] != ' ') 
			{
				cardData->primaryAccountNumber_withoutSpaces[local_noSpaceCounter++] = cardData->primaryAccountNumber[local_loopCounter];
			}
		}
		cardData->primaryAccountNumber_withoutSpaces[local_noSpaceCounter] = '\0';

		// Check if the PAN is a Luhn number
		if (isLuhnNumber(cardData->primaryAccountNumber_withoutSpaces))
		{
			return TERMINAL_OK;
		}
		else
		{
			return INVALID_CARD;
		}
	}
}
