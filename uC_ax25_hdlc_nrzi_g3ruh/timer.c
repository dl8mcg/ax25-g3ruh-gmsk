/*
 * timer.c
 *
 * Created: 18.04.2024
 * Modified: 16.01.2026
 * Author: DL8MCG
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "board.h"
#include "hdlc_irq.h"

#define prescaler 8
#define oversampling 8

volatile static uint32_t ctc_divider;

volatile static uint8_t clkcnt = 4;

void init_timer_ctc(uint32_t firq)
{
	// Timer1 konfigurieren (16-Bit Timer) im CTC-Modus
	TCCR1B |= (1 << WGM12);		// CTC-Modus aktivieren
	TCCR1B |= (1 << CS11);		// Prescaler auf 8 setzen, CS10 und CS12 auf 0 setzen
	TIMSK |= (1 << OCIE1A);		// Output Compare Match A Interrupt aktivieren
	
	// Vergleichswert für CTC-Modus setzen, damit alle 1/firq s ein Interrupt ausgelöst wird
	ctc_divider = F_CPU/prescaler/(firq*oversampling) - 1;

	OCR1A = ctc_divider;
}

ISR(TIMER1_COMPA_vect)			// IRQ CTC-Timer
{
	clkcnt--;					// bitclock divider
	if(!clkcnt)
	{
		BITCLK_TOGGLE;			// bitclock
		clkcnt = 4;
	}
	//BITCLK_HIGH;
	smFSK();
	//BITCLK_LOW;
}






