/*
 * main.c
 *
 * Created: 05.01.2026
 * Modified: 16.01.2026
 * Author: DL8MCG
 */ 

#include <xc.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdint.h>
#include "board.h"
#include "timer.h"
#include "hdlc_irq.h"
#include "hdlc.h"

volatile uint8_t packetcnt;

#define dlyval 500000
uint32_t verz;

void tunix()
{
	while(verz < dlyval)
	{
		verz++;
		wdt_reset();
	}
	verz = 0;
}

int main(void)
{
	wdt_enable(WDTO_2S);				// Watchdog auf 2 s setzen
	TX_DDR_OUT;
	FSK_DDR_OUT;
	DAC_DDR_OUT;
	BITCLK_DDR_OUT;
	RX_DDR_OUT;
	
	cli();
	init_timer_ctc(9600);				// Datenrate in bit pro sekunde

	InitHDLC();
				 
	while(1)
	{
		RX_ON;							// nur LED-Test
		SetAX25Text(
					0x01,               // type AX.25 V2
					"EU", 0,			// Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"G3RUH 9600bd Standard: Das Scrambler-Polynom 1+x^12+x^17 transformiert den Datenstrom in ein pseudo-zufaelliges Signal.\n\r"
					"# %d\n\r", packetcnt++
					);
		tunix();
		RX_OFF;							// nur LED-Test
			
		SetAX25Text(
					0x01,               // type AX.25 V2
					"EU", 0,			// Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"Dies verhindert einen DC-Offset im Modulator und garantiert die Taktsynchronisation des Empfaengers, selbst bei langen Nullfolgen im Datenbitstrom.\n\r"
					"# %d\n\r", packetcnt++
					);
		tunix();
					
		SetAX25Text(
					0x01,               // type AX.25 V2
					"QST", 0,           // Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"Auf der Empfaengerseite macht der Descrambler diesen Prozess rueckgaengig. AX.25 Pakete werden so robust gegen Taktsynchronisationsverlust geschuetzt.\n\r"
					"Entwickelt fuer den UO-13 Satelliten, ist es heute der Standard fuer 9k6 Packet Radio weltweit.\n\r"
					"# %d\n\r", packetcnt++
					);
		tunix();
								
		SetAX25Text(
					0x01,               // type AX.25 V2
					"ALL", 0,           // Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"Vor UO-13 war der Standard im Weltraum 1200 bit/s AFSK.\n\r"
					"Das war extrem langsam für die wachsenden Datenmengen.\n\r"
					"UO-13 lieferte stabile 9600 bit/s. Das war die achtfache Geschwindigkeit!\n\r"
					"# %d\n\r", packetcnt++
					);	
		tunix();
			
		SetAX25Text(
					0x01,               // type AX.25 V2
					"WORLD", 0,         // Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"James Miller (G3RUH) entwickelte das Modem speziell, um die begrenzten Bandbreiten der Satelliten-Transponder optimal zu nutzen.\n\r"
					"# %d\n\r", packetcnt++
					);
		tunix();
		
		SetAX25Text(
					0x01,               // type AX.25 V2
					"WORLD", 0,         // Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"Der Erfolg war so groß, dass Funkamateure weltweit begannen, ihre TNCs (Terminal Node Controller) mit G3RUH-Platinen nachzurüsten.\n\r"
					"# %d\n\r", packetcnt++
					);
		tunix();	
			
		SetAX25Text(
					0x01,               // type AX.25 V2
					"CQ", 0,			// Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"Es war das erste Mal, dass eine komplexe digitale Modulation (Direct FSK mit Scrambler) im Amateurbereich massentauglich wurde.\n\r"
					"UO-13 arbeitete als fliegende Mailbox.\n\r"
					"# %d\n\r", packetcnt++
					);
		tunix();
		
		SetAX25Text(
					0x01,               // type AX.25 V2
					"CQ", 0,			 // Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"Dank der 9k6-Verbindung konnten Bodenstationen in der kurzen Zeit, in der der Satellit sichtbar war (ca. 10–15 Minuten), hunderte von Nachrichten hoch- und herunterladen.\n\r"
					"# %d\n\r", packetcnt++ 
					);
		tunix();		

		SetAX25Text(
					0x01,               // type AX.25 V2
					"CQ", 0,			 // Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"Die Fehlerrate war dank des G3RUH-Scramblers extrem gering, da dieser das Signal unempfindlich gegenüber den typischen Verzerrungen im Funkkanal machte.\n\r"
					"# %d\n\r", packetcnt++ 
					);
		tunix();
		
		SetAX25Text(
					0x01,               // type AX.25 V2
					"CQ", 0,			// Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"Der Erfolg war so nachhaltig, dass fast alle nachfolgenden Digisatelliten (wie UO-22, KO-23, KO-25) das G3RUH-Verfahren übernahmen.\n\r"
					"# %d\n\r", packetcnt++
					);
		tunix();	
		
		SetAX25Text(
					0x01,               // type AX.25 V2
					"CQ", 0,			// Zielrufzeichen + SSID
					"nocall", 0,        // Quellrufzeichen + SSID
					0x03,               // Control (UI-Frame)
					0xF0,               // PID (No Layer 3)
					"AVR source code https://github.com/dl8mcg/ax25-g3ruh-gmsk\n\r"
					"# %d\n\r", packetcnt++
					);
		tunix();
	}
}
