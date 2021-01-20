/*

Credentials file

*/

#pragma once

// Only one of these settings must be defined
#define USE_ABP
//#define USE_OTAA

#ifdef USE_ABP

// UPDATE WITH YOUR TTN KEYS AND ADDR.
static const PROGMEM u1_t NWKSKEY[16] = { 0xED, 0x99, 0x59, 0x0C, 0x06, 0xCC, 0x0D, 0x01, 0xEE, 0x9B, 0x63, 0xCD, 0xD7, 0x12, 0x7F, 0xB2 };
static const u1_t PROGMEM APPSKEY[16] = { 0x72, 0xCE, 0xED, 0x15, 0x03, 0x90, 0x84, 0x87, 0x79, 0xBD, 0x88, 0x11, 0xF0, 0x26, 0xD1, 0x8D };
//static const u4_t DEVADDR = 0x26011E7B ; // <-- Change this address for every node!
static u4_t DEVADDR = 0x26011E7B ; // <-- Change this address for every node!

#endif

#ifdef USE_OTAA

    // This EUI must be in little-endian format, so least-significant-byte
    // first. When copying an EUI from ttnctl output, this means to reverse
    // the bytes. For TTN issued EUIs the last bytes should be 0x00, 0x00,
    // 0x00.
    static const u1_t PROGMEM APPEUI[8]  = { 0x47, 0x9D, 0x03, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };

    // This should also be in little endian format, see above. (least-significant-byte
    // first)
    static const u1_t PROGMEM DEVEUI[8]  = { 0xFF, 0xFE, 0x50, 0xCD, 0x57, 0x60, 0xA1, 0x24 };

    // This key should be in big endian format (or, since it is not really a
    // number but a block of memory, endianness does not really apply). In
    // practice, a key taken from ttnctl can be copied as-is.
    // The key shown here is the semtech default key.
    static const u1_t PROGMEM APPKEY[16] = { 0xDA, 0x8D, 0xDF, 0xE2, 0xC5, 0x8F, 0xC1, 0x4D, 0xF0, 0xB0, 0x42, 0xEA, 0x84, 0xC4, 0x3D, 0x3C };

#endif
