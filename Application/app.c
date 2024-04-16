/*
 * App_Program.c
 *
 *  Created on: 3 April 2024
 * 
 *      Authors: Eng.Selim Muhammed Abbas
 *               Eng.Engy Muhammed Shehata
 */

#include <stdio.h>
#include "STD_Types.h"
#include "card.h"
#include "terminal.h"
#include "server.h"
#include "app.h"

#define CARD_HOLDER_NAME_MAX_SIZE  (uint8_t) 24
#define CARD_HOLDER_NAME_MIN_SIZE  (uint8_t) 20

#define CARD_EXPIRY_DATE_MAX_SIZE  (uint8_t) 5

#define PRIMARY_ACC_NUM_MAX_SIZE  (uint8_t) 19
#define PRIMARY_ACC_NUM_MIN_SIZE  (uint8_t) 16

#define MAX_AMOUNT_OF_TERMINAL  (f32) 20000

ST_cardData_t cardData ;
ST_terminalData_t terminalData ;
ST_accountsDB_t accountRefrence ;
ST_transaction_t transactionData ;

EN_cardError_t     APP_getCardData(void);
EN_terminalError_t APP_getTerminalData(void);
EN_serverError_t   APP_getServerData(void);

void main(void)
{
	
	appStart();

}

void appStart(void)
{
	EN_cardError_t cardStatus = APP_getCardData();
	switch (cardStatus)
	{
	case CARD_OK:
		EN_terminalError_t terminalStatus = APP_getTerminalData();
		switch (terminalStatus)
		{
		case TERMINAL_OK:
			EN_serverError_t serverStatus = APP_getServerData();
			switch (serverStatus)
			{
			case SERVER_OK:
				printf("SERVER_OK");
				break;

			}

			break;

		default: printf("TERMINAL_ACCESS_DENIED"); break;
		}

		break;



	default: printf("CARD_ACCESS_DENIED"); break;
	}

}

    /*
	 *
	 *   CARD DATA APPLICATION FUNCTION
	 *
	 *
	 */

EN_cardError_t APP_getCardData(void)
{
	

	printf("Please Enter your name (Not less than %d Characters & Up to %d Characters):", CARD_HOLDER_NAME_MIN_SIZE, CARD_HOLDER_NAME_MAX_SIZE);
	// Use gets to safely read string input from the user
	gets(&cardData.cardHolderName);
	EN_cardError_t cardHolderNameState = getCardHolderName(&cardData);

	switch (cardHolderNameState)
	{
	case CARD_OK:
		printf("Please Enter your card expiry date in form of (MM/YY): ");

		// Use gets to safely read string input from the user
		gets(cardData.cardExpirationDate);
		EN_cardError_t cardExpiratonDateState = getCardExpiryDate(&cardData);
		switch (cardExpiratonDateState)
		{
		case CARD_OK:
			printf("Please Enter your Primary Account Number (Not less than %d Characters & Up to %d Characters): ", PRIMARY_ACC_NUM_MIN_SIZE, PRIMARY_ACC_NUM_MAX_SIZE);

			// Use gets to safely read string input from the user
			gets(cardData.primaryAccountNumber);
			EN_cardError_t cardPAN_State = getCardPAN(&cardData);
			switch (cardPAN_State)
			{
			case CARD_OK:  
			//	EN_terminalError_t cardPAN_LuhnState = isValidCardPAN(&cardData);
			//	switch (cardPAN_LuhnState)
			//	{
			//	case TERMINAL_OK:
					printf("CARD_OK\n");
				return CARD_OK;
				break;

			case WRONG_PAN: printf("WRONG_PAN\n"); break;
	      	//	}
			
			break;


			//case WRONG_PAN: printf("WRONG_PAN\n"); break;
			}
			break;

		case WRONG_EXP_DATE: printf("WRONG_EXP_DATE\n"); break;
		}
		break;

	case WRONG_NAME: printf("WRONG_NAME\n"); break;
	
	}

}
    /*
	 *
	 *
	   TERMINAL DATA APPLICATION FUNCTION
	 *
	 *
	 */
EN_terminalError_t APP_getTerminalData(void)
{
	
	//Setting Max Amount of Terminal 
	setMaxAmount(&terminalData, MAX_AMOUNT_OF_TERMINAL);

	printf("Please Enter the Transaction Date (in form of DD/MM/YYYY): ");
	// Use gets to safely read string input from the user
	gets(terminalData.transactionDate);
	EN_terminalError_t transactionDateState = getTransactionDate(&terminalData);
	switch (transactionDateState)
	{
	case TERMINAL_OK:

		EN_terminalError_t expirationDateState = isCardExpired(&cardData,&terminalData);

		switch (expirationDateState)
		{
		case TERMINAL_OK:
			printf("Please Enter your Transaction Amount: ");

			// Scan transAmount from user
			scanf_s("%f", &terminalData.transAmount);
			EN_terminalError_t transAmountState = getTransactionAmount(&terminalData);
			switch (transAmountState)
			{
			case TERMINAL_OK:
			EN_terminalError_t amountCheckState = isBelowMaxAmount(&terminalData);
			switch (amountCheckState)
			{
			case TERMINAL_OK:
				printf("TERMINAL_OK\n");
				return TERMINAL_OK;
				break;
			case EXCEED_MAX_AMOUNT: printf("EXCEED_MAX_AMOUNT\n"); break;
			}
				break;


			case INVALID_AMOUNT: printf("INVALID_AMOUNT\n"); break;
			}
		break;


		case EXPIRED_CARD: printf("EXPIRED_CARD\n"); break;
		}
		break;


	case WRONG_DATE: printf("WRONG_DATE\n"); break;
	}

}

   /*
	*
	*
	  SERVER DATA APPLICATION FUNCTION
	*
	*
	*/
EN_serverError_t APP_getServerData(void)
{
	EN_serverError_t transactionSaveState;

	EN_serverError_t transactionDataState = recieveTransactionData(&transactionData);
	switch (transactionDataState)
	{
	case SERVER_OK:
		EN_serverError_t accountValidationState = isValidAccount(&cardData, &accountRefrence);
		switch (accountValidationState)
		{
		case SERVER_OK:
			EN_serverError_t accountState = isBlockedAccount(&accountRefrence);
			switch (accountState)
			{
			case SERVER_OK:
				EN_serverError_t accountAmountState = isAmountAvailable(&terminalData, &accountRefrence);
				switch (accountAmountState)
				{
				case SERVER_OK:
					 transactionSaveState = saveTransaction(&transactionData);
					switch (transactionSaveState)
					{
					case SERVER_OK:
						transactionData.transState = APPROVED;
						printf("APPROVED\n");
						return APPROVED;
						break;

					case SAVING_FAILED:
						transactionData.transState = INTERNAL_SERVER_ERROR;
						printf("INTERNAL_SERVER_ERROR\n");
						return INTERNAL_SERVER_ERROR;
						break;
					}
					break;

				case LOW_BALANCE: printf("LOW_BALANCE\n"); break;
				}
				break;

			case BLOCKED_ACCOUNT: printf("BLOCKED_ACCOUNT\n"); break;
			}
			break;

		case ACCOUNT_NOT_FOUND: printf("ACCOUNT_NOT_FOUND\n"); break;
		}
	break;


	case DECLINED_INSUFFECIENT_FUND:
		 transactionSaveState = saveTransaction(&transactionData);
		break;

	case DECLINED_STOLEN_CARD:
		transactionSaveState = saveTransaction(&transactionData);
		break;

	case FRAUD_CARD:
		transactionSaveState = saveTransaction(&transactionData);
		break;

	}
	
}