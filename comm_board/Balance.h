/* 
 * File:   BalanceLogic.h
 * Author: sidor
 *
 * Created on January 22, 2015, 7:02 PM
 */

#ifndef BALANCE_H
#define	BALANCE_H

#include <stdint.h>

void initBalance(void);
uint32_t getBalance(void);
void setBalance(uint32_t new_balance);
void balanceStep(void);

#endif	/* BALANCE_H */

