/*
 * hdlc.c
 *
 * Created: 24.07.2024
 * Modified: 16.01.2026
 * Author: DL8MCG
 */ 

#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "hdlc.h"
#include "hdlc_irq.h"

static char payload_buffer[MAX_PAYLOAD_LEN];
static HDLC_Frame_t hdlc_frame;

// --- Tabellen im Flash ---
static const uint16_t crc_table[256] PROGMEM =
{
    #include "crc_ccitt_table.inc"
};

static const uint8_t bitrev[256] PROGMEM =
{
    #include "bitrev.inc"
};

// --- Hilfsfunktionen ---
static inline void HDLC_StuffByte(uint8_t val, uint8_t *output, uint16_t *out_len, 
                              uint8_t *byte, int8_t *bit_pos, uint8_t *bit_count)
{
    if (*out_len >= (MAX_FRAME_LEN - 1)) return;     // Sicherheitscheck gegen Buffer Overflow

    for (uint8_t b = 0; b < 8; b++) 
    {
        uint8_t bit = (val >> (7 - b)) & 1;

        if (bit) 
        {
            *byte |= (1 << *bit_pos);
            (*bit_count)++;
        } 
        else 
        {
            *bit_count = 0;
        }

        (*bit_pos)--;

        if (*bit_pos < 0) 
        {
            output[(*out_len)++] = *byte;
            *byte = 0;
            *bit_pos = 7;
        }

        if (*bit_count == 5) 
        {
            (*bit_pos)--; 
            if (*bit_pos < 0) 
            {
                output[(*out_len)++] = *byte;
                *byte = 0;
                *bit_pos = 7;
            }
            *bit_count = 0;
        }
    }
}

static inline void HDLC_EncodeByte(uint8_t b, uint16_t *crc, uint8_t *output, uint16_t *out_len, 
                             uint8_t *byte, int8_t *bit_pos, uint8_t *bit_count)
{
    uint8_t r = pgm_read_byte(&bitrev[b]);
    HDLC_StuffByte(r, output, out_len, byte, bit_pos, bit_count);
    uint8_t j = (r ^ (*crc >> 8)) & 0xFF;
    *crc = pgm_read_word(&crc_table[j]) ^ (*crc << 8);
}

void encode_callsign(uint8_t type, uint8_t* dest, const char* callsign, uint8_t ssid, uint8_t last)
{
    int i;
    for (i = 0; i < 6 && callsign[i]; i++)
		dest[i] = callsign[i] << 1;
    while (i < 6) 
		dest[i++] = ' ' << 1;
    dest[6] = ((ssid & 0x0F) << 1) | 0x60 | (last ? 0x01 : 0x00) | (type ? 0x80 : 0x00);
}

// --- Hauptfunktionen ---
int AX25_EncodeHDLC(const char *dest_call, uint8_t dest_ssid,
                    const char *src_call, uint8_t src_ssid,
                    uint8_t control, uint8_t pid,
                    const char *payload, uint16_t payload_len,
                    uint8_t* output)
{
    uint16_t crc = 0xFFFF;
    uint16_t out_len = 0;
    uint8_t bit_count = 0;
    int8_t bit_pos = 7;
    uint8_t byte = 0;
    uint8_t addr[7];

    // 1. Zieladresse
    encode_callsign(1, addr, dest_call, dest_ssid, 0);
    for(uint8_t i=0; i<7; i++) 
        HDLC_EncodeByte(addr[i], &crc, output, &out_len, &byte, &bit_pos, &bit_count);

    // 2. Quelladresse
    encode_callsign(0, addr, src_call, src_ssid, 1);
    for(uint8_t i=0; i<7; i++) 
        HDLC_EncodeByte(addr[i], &crc, output, &out_len, &byte, &bit_pos, &bit_count);

    // 3. Control & PID
    HDLC_EncodeByte(control, &crc, output, &out_len, &byte, &bit_pos, &bit_count);
    HDLC_EncodeByte(pid, &crc, output, &out_len, &byte, &bit_pos, &bit_count);

    // 4. Payload (Direkt aus dem übergebenen Pointer)
    for (uint16_t i = 0; i < payload_len; i++)
		HDLC_EncodeByte((uint8_t)payload[i], &crc, output, &out_len, &byte, &bit_pos, &bit_count);

    // 5. CRC
    crc ^= 0xFFFF;
    HDLC_StuffByte((crc >> 8) & 0xFF, output, &out_len, &byte, &bit_pos, &bit_count);
    HDLC_StuffByte(crc & 0xFF, output, &out_len, &byte, &bit_pos, &bit_count);

    // 6. Endflags
    for (uint8_t f = 0; f < 5; f++) 
    {
        uint8_t flag = 0x7E;
        for (int8_t i = 7; i >= 0; i--) 
        {
            if ((flag >> i) & 1) 
				byte |= (1 << bit_pos);
            bit_pos--;
            if (bit_pos < 0) 
            {
                if (out_len < MAX_FRAME_LEN) 
					output[out_len++] = byte;
                byte = 0;
                bit_pos = 7;
            }
        }
    }

    if (bit_pos != 7 && out_len < MAX_FRAME_LEN) 
        output[out_len++] = byte;

    return out_len;
}

void SetAX25Text(uint8_t type, const char *dest_call, uint8_t dest_ssid,
                 const char *src_call, uint8_t src_ssid,
                 uint8_t control, uint8_t pid, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    
    vsnprintf(payload_buffer, MAX_PAYLOAD_LEN, format, args);	// direkt in den statischen payload_buffer schreiben
    va_end(args);

    uint16_t p_len = strlen(payload_buffer);
        
    int len = AX25_EncodeHDLC(dest_call, dest_ssid, 
                              src_call, src_ssid, 
                              control, pid, 
                              payload_buffer, p_len, 
                              hdlc_frame.data);
        
    if(len > 0)
		SendHDLC(hdlc_frame.data, len);
}