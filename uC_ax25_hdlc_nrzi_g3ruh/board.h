/*
 * board.h
 *
 * Created: 29.06.2024 
 * Modified: 16.01.2026
 * Author: DL8MCG
 */ 

#ifndef BOARD_H_
#define BOARD_H_

#define F_CPU 16000000UL   

/*****  Pin-Zuordnung  ATmega32  *********************************************/
/*****************************************************************************/

#define DAC_DDR_OUT			DDRB  |= 0xFF
#define DAC					PORTB

#define FSK					_BV(PD2)
#define FSK_DDR_OUT			DDRD  |= FSK
#define FSK_HIGH			PORTD |= FSK
#define FSK_LOW				PORTD &= ~FSK
#define FSK_TOGGLE			PORTD ^= FSK
#define FSK_DDR_IN			DDRD  &= ~FSK
#define FSK_DDR_IN_HighZ	do{DDRD  &= ~FSK; PORTB &= ~FSK;}while(0)
	
#define FSK_MASK (1<<PD2)
#define FSKOUT				do { PORTD = (PORTD & ~FSK_MASK) | (freq << PD2); } while (0)

#define BITCLK				_BV(PD3)				
#define BITCLK_DDR_OUT		DDRD  |= BITCLK
#define BITCLK_HIGH			PORTD |= BITCLK
#define BITCLK_LOW			PORTD &= ~BITCLK
#define BITCLK_TOGGLE		PORTD ^= BITCLK

#define TX					_BV(PD6)				
#define TX_DDR_OUT			DDRD  |= TX
#define TX_ON				PORTD |= TX
#define TX_OFF				PORTD &= ~TX
#define TX_TOGGLE			PORTD ^= TX

#define RX					_BV(PD7)
#define RX_DDR_OUT			DDRD  |= RX
#define RX_ON				PORTD |= RX
#define RX_OFF				PORTD &= ~RX
#define RX_TOGGLE			PORTD ^= RX

#endif /* BOARD_H_ */