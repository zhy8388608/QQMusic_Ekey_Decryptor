#include "common.h"

unsigned char mapL(unsigned __int64 place) {
	unsigned __int64 iv;
	unsigned __int8 v1;
	char v2;
	char v3;

	if ( place > 0x7FFF )
		place %= 0x7FFF;
	iv = (place * place + 71214) % keyLen;
	v1 = keyMap[iv];
	v2 = iv & 7;
	if ( v2 <= 3 )
		v3 = v2 + 4;
	else
		v3 = v2 - 4;
	return (v1 << v3 | v1 >> v3) & 0xff;
}

void ProcessByMapL(unsigned __int64 place, char *xorMap, unsigned __int64 decSize) {
	unsigned __int64 i;
	for(i = 0; i < decSize; i++)
		xorMap[i] = mapL(place + i);
}

