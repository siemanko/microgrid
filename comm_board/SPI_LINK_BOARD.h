/* 
 * File:   SPI_LINK_BOARD.h
 * Author: Dan
 *
 * Created on December 17, 2014, 12:24 AM
 */

#ifndef SPI_LINK_BOARD_H
#define	SPI_LINK_BOARD_H

void initSPI_LinkBoard();
void sendFloatLinkBoard(float data);
void sendIntLinkBoard(int i);
int pollLinkBoard();

int writeSPI3(int i);
void __attribute__((__interrupt__,auto_psv)) _SPI3Interrupt(void);

#endif	/* SPI_LINK_BOARD_H */

