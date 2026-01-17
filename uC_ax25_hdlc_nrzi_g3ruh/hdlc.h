/*
 * hdlc.h
 *
 * Created: 12.05.2024
 * Modified: 16.01.2026
 * Author: DL8MCG
 */

#ifndef HDLC_H_
#define HDLC_H_

#include <stdint.h>

#define MAX_PAYLOAD_LEN   256 
#define MAX_FRAME_LEN     512  

// --- Strukturen ---
typedef struct
{
    uint8_t data[MAX_FRAME_LEN];           // Der fertige HDLC-Bitstrom
    uint16_t length;                        
} HDLC_Frame_t;

// --- Funktionsprototypen ---
int AX25_EncodeHDLC(const char *dest_call, uint8_t dest_ssid,
                    const char *src_call, uint8_t src_ssid,
                    uint8_t control, uint8_t pid,
                    const char *payload, uint16_t payload_len,
                    uint8_t* output);

void SetAX25Text(uint8_t type, const char *dest_call, uint8_t dest_ssid,
                 const char *src_call, uint8_t src_ssid,
                 uint8_t control, uint8_t pid, const char *format, ...);

#endif /* HDLC_H_ */