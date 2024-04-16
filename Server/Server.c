/*
 * Server_Program.c
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
#include "server.h"

#define NULL  (void*) 0
#define ACCOUNTS_ARE_EQUAL      (uint8_t) 0
#define NUMBER_OF_USED_ACCOUNTS (uint8_t) 10
#define DATA_BASES_MAX_ELEMENTS (uint16_t)   255

extern ST_cardData_t cardData; 
extern ST_terminalData_t terminalData;
extern ST_accountsDB_t accountRefrence;
extern ST_transaction_t transactionData;

uint8_t global_u8AccountsCounter = 0;
uint32_t global_u32NewTransactionSequenceNumber = 0;

// Intializing elements of array with  NUMBER_OF_USED_ACCOUNTS
// Fill in each element of the array with {Balance ,State(Running or Blocked) ,PAN }
ST_accountsDB_t accountsDB[NUMBER_OF_USED_ACCOUNTS] = {
	{2000.0, RUNNING,   "8989374615436851"},
	{100000.0, BLOCKED, "5807007076043875"},
	{150000.0, RUNNING, "2878322439867674"},
	{75000.0, BLOCKED,  "3981245387584963"},
	{890000.0, RUNNING, "6928224398695174"},
	{6200000.0, BLOCKED,"7139445289675096"},
	{900000.0, RUNNING, "4928123546998042"},
	{800000.0, RUNNING, "2468103189580923"},
	{400000.0, RUNNING, "5246113287561969"},
	{600000.0, RUNNING, "6928113287584963"}
};


// Intializing elements of array with zero 
ST_transaction_t transactionsDB[DATA_BASES_MAX_ELEMENTS] = {{{0}}};




EN_serverError_t isValidAccount(ST_cardData_t* cardData, ST_accountsDB_t* accountReference)
{
	uint8_t local_u8AccountsCounter;
	for (local_u8AccountsCounter = 0; local_u8AccountsCounter < NUMBER_OF_USED_ACCOUNTS; local_u8AccountsCounter++)
	{
		if (strcmp(accountsDB[local_u8AccountsCounter].primaryAccountNumber, cardData->primaryAccountNumber) == 0)
		{
			global_u8AccountsCounter = local_u8AccountsCounter;
			break;
		}
	}

	if (local_u8AccountsCounter != NUMBER_OF_USED_ACCOUNTS)
	{
		accountReference->state = accountsDB[global_u8AccountsCounter].state;
		return SERVER_OK;
	}
	else
	{
		return ACCOUNT_NOT_FOUND;
	}
}

EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence)
{
		switch(accountRefrence->state) 
		{
		case BLOCKED: return BLOCKED_ACCOUNT; break;
		case RUNNING: return SERVER_OK; break;
		}
}


EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t* accountRefrence)
{	
		if (termData->transAmount < accountsDB[global_u8AccountsCounter].balance)
		{
			return SERVER_OK;
		}
		else
		{
			return LOW_BALANCE;
		}
}

EN_transState_t recieveTransactionData(ST_transaction_t* transData)
{
	//Initialize Transaction Sequence Number
	transData->transactionSequenceNumber = 0;

	//Checking account validation
	EN_serverError_t accountValidationState = isValidAccount(&cardData, &accountRefrence);
	    switch(accountValidationState)
		{
		case SERVER_OK:	
			// Saving Card Data
			strcpy_s((transData->cardHolderData.cardHolderName), sizeof(transData->cardHolderData.cardHolderName), (cardData.cardHolderName));
			strcpy_s((transData->cardHolderData.primaryAccountNumber), sizeof(transData->cardHolderData.primaryAccountNumber), (cardData.primaryAccountNumber));
			strcpy_s((transData->cardHolderData.cardExpirationDate), sizeof(transData->cardHolderData.cardExpirationDate), (cardData.cardExpirationDate));
			EN_serverError_t accountState = isBlockedAccount(&accountRefrence);
			switch (accountState)
			{
			case SERVER_OK:
				EN_serverError_t accountAmountState = isAmountAvailable(&terminalData, &accountRefrence);
				switch (accountAmountState)
				{
				case SERVER_OK:
					// Saving Terminal Data
					transData->terminalData.maxTransAmount = terminalData.maxTransAmount;
					transData->terminalData.transAmount = terminalData.transAmount;
					strcpy_s((transData->terminalData.transactionDate),sizeof(transData->terminalData.transactionDate),(terminalData.transactionDate));

					//Transaction end successfully
					accountRefrence.state = accountsDB[global_u8AccountsCounter].state;
					accountsDB[global_u8AccountsCounter].balance -= terminalData.transAmount;
					accountRefrence.balance = accountsDB[global_u8AccountsCounter].balance;

					printf("Your Transaction has been processed Successfully\n");
					printf("Your Current Balance = %f\n", accountRefrence.balance);

					//Saving the new balance in database
					accountsDB[global_u8AccountsCounter].balance = accountRefrence.balance;

					
					//Saving transaction Secuence number
					transactionsDB[transData->transactionSequenceNumber].transactionSequenceNumber = transData->transactionSequenceNumber;
					return SERVER_OK;
				break;


				case LOW_BALANCE :
					transData->transState = DECLINED_INSUFFECIENT_FUND;
					printf("DECLINED_INSUFFECIENT_FUND\n");

		
					//Saving transaction Secuence number
					transactionsDB[transData->transactionSequenceNumber].transactionSequenceNumber = transData->transactionSequenceNumber;
					return DECLINED_INSUFFECIENT_FUND;
					break;
				}
				break;


			case BLOCKED_ACCOUNT:
				transData->transState = DECLINED_STOLEN_CARD;
				printf("DECLINED_STOLEN_CARD\n");

				
				//Saving transaction Secuence number
			  transactionsDB[transData->transactionSequenceNumber].transactionSequenceNumber = transData->transactionSequenceNumber;
				return DECLINED_STOLEN_CARD;
				break;
			}
			break;


		case ACCOUNT_NOT_FOUND:
			transData->transState = FRAUD_CARD;
			printf("FRAUD_CARD\n");

			//Saving transaction Secuence number
			transactionsDB[transData->transactionSequenceNumber].transactionSequenceNumber = transData->transactionSequenceNumber;
			return FRAUD_CARD;
			break;
		}
	

}
EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
	
	for (uint16_t local_u16Counter = 0 ; local_u16Counter < DATA_BASES_MAX_ELEMENTS; local_u16Counter++)
	{
		if (transactionsDB[local_u16Counter].transactionSequenceNumber > transData->transactionSequenceNumber)
		{
			transData->transactionSequenceNumber = transactionsDB[local_u16Counter].transactionSequenceNumber;
			break;
		}
	}   
	    // Saving the new transaction sequence number 
	    global_u32NewTransactionSequenceNumber = transData->transactionSequenceNumber + 1;
		transactionsDB[global_u32NewTransactionSequenceNumber].transactionSequenceNumber = global_u32NewTransactionSequenceNumber;

		// Saving transaction data into transaction DataBase
		transactionsDB[global_u32NewTransactionSequenceNumber].transState = transData->transState;
		transactionsDB[global_u32NewTransactionSequenceNumber].terminalData.transAmount = transData->terminalData.transAmount;
		transactionsDB[global_u32NewTransactionSequenceNumber].terminalData.maxTransAmount = transData->terminalData.maxTransAmount;

		strcpy_s((transactionsDB[global_u32NewTransactionSequenceNumber].terminalData.transactionDate),sizeof(transactionsDB[global_u32NewTransactionSequenceNumber].terminalData.transactionDate), (transData->terminalData.transactionDate));
		strcpy_s((transactionsDB[global_u32NewTransactionSequenceNumber].cardHolderData.cardHolderName), sizeof(transactionsDB[global_u32NewTransactionSequenceNumber].cardHolderData.cardHolderName), (transData->cardHolderData.cardHolderName));
		strcpy_s((transactionsDB[global_u32NewTransactionSequenceNumber].cardHolderData.primaryAccountNumber), sizeof(transactionsDB[global_u32NewTransactionSequenceNumber].cardHolderData.primaryAccountNumber), (transData->cardHolderData.primaryAccountNumber));
		strcpy_s((transactionsDB[global_u32NewTransactionSequenceNumber].cardHolderData.cardExpirationDate), sizeof(transactionsDB[global_u32NewTransactionSequenceNumber].cardHolderData.cardExpirationDate), (transData->cardHolderData.cardExpirationDate));
		listSavedTransactions();

		//Assuming that connection between Server & Terminal is always connected
		return SERVER_OK;
}
	
	


void listSavedTransactions(void)
{
	printf("####################################################\n");
	printf("Transaction Sequence Number : %d\n", global_u32NewTransactionSequenceNumber);
	printf("Transaction Date : %s\n", transactionsDB[global_u32NewTransactionSequenceNumber].terminalData.transactionDate);
	printf("Transaction Amount : %f\n", transactionsDB[global_u32NewTransactionSequenceNumber].terminalData.transAmount);
	switch(transactionsDB[global_u32NewTransactionSequenceNumber].transState)
	{
	case APPROVED:
		printf("Transaction State : APPROVED\n");
		break;

	case DECLINED_INSUFFECIENT_FUND:
		printf("Transaction State : DECLINED_INSUFFECIENT_FUND\n");
		break;

	case DECLINED_STOLEN_CARD:
		printf("Transaction State : DECLINED_STOLEN_CARD\n");
		break;

	case FRAUD_CARD:
		printf("Transaction State : FRAUD_CARD\n");
		break;

	case INTERNAL_SERVER_ERROR:
		printf("Transaction State : INTERNAL_SERVER_ERROR\n");
		break;
	}
	
	printf("Terminal Max Amount : %f\n", transactionsDB[global_u32NewTransactionSequenceNumber].terminalData.maxTransAmount);
	printf("Cardholder Name :  %s\n", transactionsDB[global_u32NewTransactionSequenceNumber].cardHolderData.cardHolderName);
	printf("PAN :  %s\n", transactionsDB[global_u32NewTransactionSequenceNumber].cardHolderData.primaryAccountNumber);
	printf("Card Expiration Date :  %s\n", transactionsDB[global_u32NewTransactionSequenceNumber].cardHolderData.cardExpirationDate);
	printf("####################################################\n");
}

