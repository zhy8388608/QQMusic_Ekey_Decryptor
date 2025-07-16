#include "common.h"

unsigned int key1;
unsigned char key2[1024];

void GetHashBase() {
	unsigned int v1;
	int i;
	int v3;

	v1 = 1;
	for(i = 0; i < keyLen; i++) {
		v3 = keyMap[i];
		if(v3) {
			if(v1 * v3 <= v1)
				break;
			v1 *= v3;
		}
	}
	key1 = v1;
}

void swap(char *a, char *b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void InitRC4KSA() {
	int v3;
	int i;
	int j;

	v3 = 0;
	for(i = 0; i < keyLen; i++)
		key2[i] = i;
	for(j = 0; j < keyLen; j++) {
		v3 = (key2[j] + v3 + keyMap[j % keyLen]) % keyLen;
		swap(&key2[j], &key2[v3]);
	}

	GetHashBase(keyMap, keyLen);
}

void EncFirstSegment(unsigned __int64 place, char *dataStream) {
	int i;
	for(i = place; i < 128; i++)
		dataStream[i] ^= keyMap[(unsigned __int64)((double)key1 / (double)((i + 1) * keyMap[i % keyLen]) * 100.0) % keyLen];
}

void EncASegment(unsigned __int64 place, char *dataStream, unsigned int decSize) {
	int v6;
	int v13;
	int v14;
	unsigned int i;
	unsigned int j;
	int v17;
	unsigned int v18;

	unsigned char newKey[1024];
	memcpy(newKey, key2, keyLen);

	v17 = (place / 5120) & 0x1FF;
	if(keyMap && keyLen > v17) {
		v18 = ((unsigned __int64)((double)key1 / (double)((place / 5120 + 1) * keyMap[v17]) * 100.0) & 0x1FF) + place % 5120;
		v13 = 0;
		v14 = 0;
		for(i = 0; i < v18; i++) {
			v13 = (v13 + 1) % keyLen;
			v14 = (newKey[v13] + v14) % keyLen;
			swap(&newKey[v13], &newKey[v14]);
		}
		for(j = 0; j < decSize; j++) {
			v13 = (v13 + 1) % keyLen;
			v14 = (newKey[v13] + v14) % keyLen;
			swap(&newKey[v13], &newKey[v14]);
			v6 = newKey[v13] + newKey[v14];
			dataStream[j] ^= newKey[v6 % keyLen];
		}
	}
}

void ProcessByRC4(unsigned __int64 place, char *dataStream, unsigned __int64 decSize) {
	unsigned __int64 dataStreamIv = place;
	if(place < 128) {
		EncFirstSegment(place, dataStream);
		decSize -= 128 - place;
		place = 128;
	}
	while(decSize) {
		unsigned __int64 end = (place / 5120 + 1) * 5120;
		if(end > place + decSize)
			end = place + decSize;
		EncASegment(place, dataStream + place - dataStreamIv, end - place);
		decSize -= end - place;
		place = end;
	}
}

