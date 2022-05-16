#pragma once 
#include <vector>

const int bits_per_char = 8;

class Bitmap
{
public:
    Bitmap() {
        clear();
    }

    ~Bitmap() {
        clear();
    }

    int set(uint32_t position) {
        return _set(position, true);
    }

    int reset(uint32_t position) {
        return _set(position, false);
    }

    int clear() {
        m_bitset.clear();
        m_used_bits = 0;
        return 0;
    }

    int test(uint32_t position) {
        uint32_t char_offset = position / bits_per_char;
        uint32_t bit_offset = position % bits_per_char;
        if (char_offset >= m_bitset.size()) {
            return 0;
        }
        return (m_bitset[char_offset] & ((uint8_t)1 << bit_offset));
    }
protected:
    int _set(uint32_t position, bool do_set) {
        uint32_t char_offset = position / bits_per_char;
        uint32_t bit_offset = position % bits_per_char;

        if (char_offset >= m_bitset.size()) {
            m_bitset.resize(char_offset + 1);
        }

        if (do_set) {
            m_bitset[char_offset] |= ((uint8_t)1 << bit_offset);
        }
        else {
            m_bitset[char_offset] -= ((uint8_t)1 << bit_offset);
        }
        return 0;
    }

    int _calc_sparse_radio() {
        //> TODO: optimize the used memory
        return -1;
    }

protected:
    int                  m_used_bits { 0 };
    std::vector<uint8_t> m_bitset;
};