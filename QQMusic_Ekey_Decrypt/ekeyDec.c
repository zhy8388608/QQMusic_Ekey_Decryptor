#include "common.h"

static inline uint32_t bswap32(uint32_t value) {
	return ((value & 0x000000FFU) << 24) |
	       ((value & 0x0000FF00U) << 8) |
	       ((value & 0x00FF0000U) >> 8) |
	       ((value & 0xFF000000U) >> 24);
}

unsigned int * TeaDecryptECB(unsigned int *ciphertext, const unsigned __int8 *key, unsigned __int8 *out) {
	unsigned int v3;
	unsigned int v4;
	int v5;
	int i;
	int j;
	int v8[5];

	v3 = bswap32(*ciphertext);
	v4 = bswap32(ciphertext[1]);
	for ( i = 0; i <= 3; ++i )
		v8[i] = bswap32(*(int *)&key[4 * i]);
	v5 = -478700656;
	for ( j = 0; j <= 15; ++j ) {
		v4 -= (16 * v3 + v8[2]) ^ (v3 + v5) ^ ((v3 >> 5) + v8[3]);
		v3 -= (16 * v4 + v8[0]) ^ (v4 + v5) ^ ((v4 >> 5) + v8[1]);
		v5 += 1640531527;
	}
	*(int *)out = bswap32(v3);
	*((int *)out + 1) = bswap32(v4);
	return ciphertext;
}

int oi_symmetry_decrypt2(
    unsigned __int8 *ciphertext,
    int ciphertextLen,
    const unsigned __int8 *key,
    unsigned __int8 *out,
    int *outlen) {
	unsigned __int8 *v6;
	unsigned __int8 *v12;
	int v13;
	unsigned __int8 *v14;
	unsigned __int8 *v15;
	int v16;
	int v17;
	int i;
	int v19;
	int v20;
	int v24;
	int v25;
	unsigned __int8 v26[8];
	char v27[24];

#define PROCESS_BLOCK() \
	if ( v16 == 8 ) { \
		v14 = v15; \
		v15 = v12; \
		for ( i = 0; i <= 7; i++ ) { \
			if ( v24 + i >= ciphertextLen ) \
				return 0; \
			v26[i] ^= v12[i]; \
		} \
		TeaDecryptECB((unsigned int *)v26, key, v26); \
		v12 += 8; \
		v24 += 8; \
		v16 = 0; \
	}

	if ( (ciphertextLen & 7) != 0 || ciphertextLen <= 15 )
		return 0;
	TeaDecryptECB((unsigned int *)ciphertext, key, v26);
	v25 = v26[0] & 7;
	v17 = ciphertextLen - 1 - v25 - 9;
	if ( *outlen < v17 || v17 < 0 )
		return 0;
	*outlen = v17;
	for ( i = 0; i <= 7; ++i )
		v27[i] = 0;
	v14 = v27;
	v15 = ciphertext;
	v12 = ciphertext + 8;
	v24 = 8;
	v16 = v25 + 1;
	v19 = 1;
	while ( v19 <= 2 ) {
		if ( v16 > 7 ) {
			PROCESS_BLOCK();
		} else {
			++v16;
			++v19;
		}
	}
	v13 = *outlen;
	while ( v13 ) {
		if ( v16 > 7 ) {
			PROCESS_BLOCK();
		} else {
			v6 = out++;
			*v6 = v26[v16] ^ v14[v16];
			++v16;
			--v13;
		}
	}
	v20 = 1;
	while ( v20 <= 7 ) {
		if ( v16 > 7 ) {
			PROCESS_BLOCK();
		} else {
			if ( v26[v16] != v14[v16] )
				return 0;
			++v16;
			++v20;
		}
	}

#undef PROCESS_BLOCK
	return 1;
}

static const unsigned char base64_suffix_map[128] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255,
	255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255,
	255, 254, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
	19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
	37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 255, 255, 255, 255, 255
};

int base64_decode(const char *indata, int inlen, char *outdata, int *outlen) {

	int ret = 0;
	if (indata == NULL || inlen <= 0 || outdata == NULL || outlen == NULL) {
		return ret = -1;
	}
	if (inlen % 4 != 0) {
		return ret = -2;
	}

	int t = 0, x = 0, y = 0, i = 0;
	unsigned char c = 0;
	int g = 3;

	while (x < inlen) {
		c = base64_suffix_map[indata[x++]];
		if (c == 255) return -1;
		if (c == 253) continue;
		if (c == 254) {
			c = 0;
			g--;
		}
		t = (t << 6) | c;
		if (++y == 4) {
			outdata[i++] = (unsigned char)((t >> 16) & 0xff);
			if (g > 1) outdata[i++] = (unsigned char)((t >> 8) & 0xff);
			if (g > 2) outdata[i++] = (unsigned char)(t & 0xff);
			y = t = 0;
		}
	}
	if (outlen != NULL) {
		*outlen = i;
	}
	return ret;
}

/*
int SimpleMakeKey(char *out) {
	unsigned __int8 magic = 0x6a;
	int len = 8;
	int i;
	for ( i = 0; i < len ; i++ )
		out[i] = (unsigned int)(fabs(tan((float)magic + (double)i * 0.1)) * 100.0);
	return i;
}
*/

void makeKey(const char *head, char *key) {
	char keyHelper[] = {0x69, 0x56, 0x46, 0x38, 0x2b, 0x20, 0x15, 0x0b}; // SimpleMakeKey
	int i;
	for ( i = 0; i <= 0xF; ++i ) {
		if ( (i & 1) != 0 )
			key[i] = head[i >> 1];
		else
			key[i] = keyHelper[i >> 1];
	}
}

int decryptEkey(const char *indata, int inlen, char *outdata, int *outlen) {
	char temp[1024];
	int templen;
	base64_decode(indata, inlen, temp, &templen);
	char head[8];
	memcpy(head, temp, 8);
	memcpy(outdata, head, 8);
	char key[16];
	makeKey(head, key);
	int res = oi_symmetry_decrypt2(temp + 8, templen - 8, key, outdata + 8, outlen);
	*outlen += 8;
	return res;
}

