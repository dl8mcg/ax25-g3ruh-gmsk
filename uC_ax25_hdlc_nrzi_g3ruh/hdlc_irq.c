/*
 * hdlc_irq.c
 *
 * Created: 12.05.2024
 * Modified: 16.01.2026
 * Author: DL8MCG
 */ 

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <util/atomic.h>
#include "hdlc_irq.h"
#include "board.h"

static const uint8_t *tx_data_ptr = 0;

uint16_t byte_pos; 
uint8_t HDLCbyte;

void HDLC_Start();
void HDLC_SendFlags();
void HDLC_Run();
void HDLC_Stop();
void (* volatile smFSK)(void) = HDLC_Stop;		// Funktionszeiger

static uint8_t flagbyte = 0x7E;
static uint16_t flagcnt;

bool isHDLC();

uint8_t bitcnt;
uint16_t bytecnt;

uint8_t subcnt;
uint8_t ls;

static volatile uint8_t freq;

static volatile uint16_t g3ruh_lsr = 0x1000;
static volatile uint8_t  g3ruh_msb = 1;

#define NS 8

const uint8_t PROGMEM LUT[8][NS] =
{
    {   0,    0,    0,    0,    0,    0,    0,    0 },
    {   0,    0,    0,    2,    6,   18,   42,   80 },
    { 127,  175,  212,  235,  243,  235,  212,  175 },
    { 128,  175,  213,  237,  249,  253,  255,  255 },
    { 127,   80,   42,   18,    6,    2,    0,    0 },
    { 128,   80,   43,   20,   12,   20,   43,   80 },
    { 255,  255,  255,  253,  249,  237,  213,  175 },
    { 255,  255,  255,  255,  255,  255,  255,  255 }
};

// ---------------------------------------------------------
// G3RUH & NRZI
// ---------------------------------------------------------
static inline uint8_t __attribute__((always_inline)) HDLC_CalcBit(uint8_t data_in)
{
    uint8_t bit_in = (data_in >> 7) & 1;

    // G3RUH
    uint8_t fb = ((g3ruh_lsr >> 11) ^ g3ruh_msb) & 1;
    uint8_t bit_out = bit_in ^ fb;

    g3ruh_msb = (g3ruh_lsr >> 15) & 1;
    g3ruh_lsr = (g3ruh_lsr << 1) | bit_out;

    // NRZI
    freq ^= (bit_out == 0);
    
    // Daten shiften für nächsten Durchlauf
    data_in <<= 1;

    // Zähler und State Updates
    bitcnt = (bitcnt + 1) & 7;
    ls = ((ls << 1) | freq) & 7;
    
    FSKOUT;
    
    return data_in; // Gibt das geshiftete Byte zurück
}

void InitHDLC()
{
    bitcnt = 0;
    subcnt = 0;
    freq = 0;
    ls = 0;
    cli();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    { smFSK=HDLC_Stop; }      
}

void SendHDLC(const uint8_t * buf, uint16_t size)
{
    tx_data_ptr = buf;
    
    byte_pos = 0; 
    cli();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    { smFSK = HDLC_Start; }
		
    bitcnt = 0;
    flagbyte = 0x7E;
    flagcnt = 50; 
        
    g3ruh_lsr = 0x1000;
    g3ruh_msb = 1;

    bytecnt = size;

    sei();
    while(isHDLC())
    {
    }
}
        
void HDLC_Stop()
{
    TX_OFF;
}

bool isHDLC()
{
    wdt_reset();
    return !(smFSK == HDLC_Stop);
}

void HDLC_Start()
{
    TX_ON;
    smFSK = HDLC_SendFlags;
}

void HDLC_SendFlags()
{
    if(subcnt == 0)
    {
        // Aufruf der Inline Funktion
        flagbyte = HDLC_CalcBit(flagbyte);

        // Spezifische Logik für Flags
        if(bitcnt == 0)
        {
            flagcnt--;
            if(flagcnt == 0) 
				smFSK = HDLC_Run;
            else 
				flagbyte = 0x7E;
        }
    }

    DAC = pgm_read_byte(&LUT[ls][subcnt]);
    subcnt = (subcnt + 1) & 7;
}

void HDLC_Run()
{
    if(subcnt == 0)
    {
        // Spezifische Logik für Daten-Load
        if(bitcnt == 0)
        {
            HDLCbyte = tx_data_ptr[byte_pos++];
            bytecnt--;
        }

        // Aufruf der Inline Funktion
        HDLCbyte = HDLC_CalcBit(HDLCbyte);
    }

    DAC = pgm_read_byte(&LUT[ls][subcnt]);
    subcnt = (subcnt + 1) & 7;
    
    // Prüfen ob fertig
    if(!bytecnt && !bitcnt)
    {
        smFSK = HDLC_Stop;
        return;
    }
}