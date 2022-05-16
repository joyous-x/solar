#include "crc16.h"

#define CRC_TABLE_SIZE                       256

void make_crc_talbe(unsigned int polynomial, unsigned int CRCTable[]) {
    unsigned int I, J;
    unsigned int HashValue;

    for (I = 0; I <= 255; I++) {
        for (HashValue = I, J = 8; J; J--) {
            HashValue = (HashValue & 1) ? (HashValue >> 1) ^ polynomial : (HashValue >> 1);
        }
        CRCTable[I] = HashValue;
    }
}

unsigned short crc16(unsigned crc, const unsigned char* data, int size, unsigned polynomial) {
    unsigned int crc16_table[CRC_TABLE_SIZE] = { 0 };
    make_crc_talbe(polynomial, crc16_table);

    while (size--) {
        crc = ((crc >> 8) ^ crc16_table[(crc ^ (*data++)) & 0xff]);
    }
    return crc;
}