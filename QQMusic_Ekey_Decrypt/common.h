#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern unsigned char keyMap[];
extern unsigned int keyLen;

int decryptEkey(const char *indata, int inlen, char *outdata, int *outlen);
void InitRC4KSA();
void ProcessByMapL(unsigned __int64 place, char *xorMap, unsigned __int64 decSize);
void ProcessByRC4(unsigned __int64 place, char *xorMap, unsigned __int64 decSize);

