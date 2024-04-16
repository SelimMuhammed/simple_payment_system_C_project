# Application Logic Overview

This document provides an overview of the application logic implemented in the provided C code.

## 1. Introduction

The code consists of functions that simulate a transaction process involving a card, terminal, and server. Each function handles specific aspects of the transaction process.

## 2. Function Descriptions

### `main()`

Entry point of the program. Calls the `appStart()` function to initiate the transaction process.

### `appStart()`

Initiates the transaction process by calling functions to get card data, terminal data, and server data. Handles errors at each step.

### `APP_getCardData()`

Function to collect card data from the user including cardholder name, card expiry date, and primary account number. Validates each input and returns appropriate error states.

### `APP_getTerminalData()`

Collects terminal data including transaction date and amount from the user. Validates inputs and checks for card expiry. Returns appropriate error states.

### `APP_getServerData()`

Collects server data including transaction details. Validates account information, checks for account validity and balance, and saves the transaction. Returns appropriate error states.

## 3. Error Handling

Error states are returned by each function to indicate the success or failure of specific operations. These states include:

- `CARD_OK`, `TERMINAL_OK`, `SERVER_OK`: Indicates successful completion of card, terminal, or server data retrieval respectively.
- Various error states such as `WRONG_NAME`, `WRONG_EXP_DATE`, `WRONG_PAN`, `EXPIRED_CARD`, etc., indicate specific issues encountered during data retrieval or validation.
- `APPROVED`: Indicates successful completion of the transaction.
- `INTERNAL_SERVER_ERROR`, `LOW_BALANCE`, `BLOCKED_ACCOUNT`, `ACCOUNT_NOT_FOUND`, etc., indicate server-side errors or issues related to account status or balance.

## 4. Constants

Several constants are defined at the beginning of the code to specify constraints such as maximum sizes for cardholder name, card expiry date, primary account number, and maximum amount of terminal.

## 5. Conclusion

The provided code implements a basic transaction processing system with error handling at each step of the process. It collects data from the user, validates it, and simulates interaction between a card, terminal, and server.