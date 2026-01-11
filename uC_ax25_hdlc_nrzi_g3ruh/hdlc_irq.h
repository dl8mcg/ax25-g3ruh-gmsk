/*
 * hdlc_irq.h
 *
 * Created: 12.05.2024
 * Modified: 25.12.2025
 * Author: DL8MCG
 */ 

#ifndef AX25_H_
#define AX25_H_

void (* volatile smFSK)(void);		// Funktionszeiger

void InitHDLC();

void SendHDLC(const uint8_t * buf, uint16_t size);

#endif /* AX25_H_ */