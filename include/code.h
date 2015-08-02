#ifndef _CODE_H
#define _CODE_H
void code16(unsigned char*s, int value);
void code32(unsigned char*s, int value);
int uncode32(unsigned char**s);
int uncode16(unsigned char**s);

#endif
