#include "include/tm.h"
/* code two bytes to string, Big-Endian */
void code16(unsigned char* src, int value) {
    src[0] = (value >> 8) & 0xff;
    src[1] = (value) & 0xff;
}
int uncode16(unsigned char** src) {
    unsigned char* s = *src;
    *src += 2;
    return (s[0] << 8) + (s[1] & 0xff);
}

void code32(unsigned char* src, int value) {
    src[0] = (value >> 20) & 0xff;
    src[1] = (value >> 16) & 0xff;
    src[2] = (value >> 8) & 0xff;
    src[3] = (value) & 0xff;
}
int uncode32(unsigned char** src) {
    unsigned char* s = *src;
    *src += 4;
    return (s[0]<<20) + (s[1]<<16) + (s[2]<<8) + (s[3]);
}