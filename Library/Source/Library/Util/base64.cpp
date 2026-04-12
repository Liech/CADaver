#include "base64.h"

namespace Library
{
    std::string base64::base64_encode(const unsigned char* data, size_t len)
    {
        static const char lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string       out;
        for (size_t i = 0; i < len; i += 3)
        {
            uint32_t val = (data[i] << 16) | ((i + 1 < len ? data[i + 1] : 0) << 8) | (i + 2 < len ? data[i + 2] : 0);
            out.push_back(lookup[(val >> 18) & 0x3F]);
            out.push_back(lookup[(val >> 12) & 0x3F]);
            out.push_back(i + 1 < len ? lookup[(val >> 6) & 0x3F] : '=');
            out.push_back(i + 2 < len ? lookup[val & 0x3F] : '=');
        }
        return out;
    }

    std::vector<unsigned char> base64::base64_decode(const std::string& in)
    {
        static const int           table[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                               -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
                                               -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
                                               -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 };
        std::vector<unsigned char> out;
        uint32_t                   val  = 0;
        int                        valb = -18;
        for (unsigned char c : in)
        {
            if (c == '=')
                break;
            if (table[c] == -1)
                continue;
            val = (val << 6) | table[c];
            valb += 6;
            if (valb >= 0)
            {
                out.push_back((val >> valb) & 0xFF);
                valb -= 8;
            }
        }
        return out;
    }
}