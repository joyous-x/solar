#pragma once 

#define BASE_CRC_MASK                        0xFFFFFFFFL
#define BASE_CRC_POLYNOMIAL                  0xEDB88320L

/**
 * crc16 - compute the CRC-16 for the data buffer
 * @crc:	previous CRC value
 * @data:	data pointer
 * @size:	number of bytes in the buffer
 *
 * Returns the updated CRC value.
 */
unsigned short crc16(unsigned crc, const unsigned char* data, int size, unsigned polynomial = BASE_CRC_POLYNOMIAL);