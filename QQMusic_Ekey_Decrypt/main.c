#include "common.h"

#define BUFFER_SIZE 1024

unsigned char keyMap[1024];
unsigned int keyLen = 1024;

void set_ext(char *filename, const char *new_ext) {
	char *dot = strrchr(filename, '.');
	if (dot) strcpy(dot , new_ext);
	else strcat(filename, new_ext);
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf("Usage: %s [file] [ekey]\n", argv[0]);
		return -1;
	}
	int success = decryptEkey(argv[2], strlen(argv[2]), keyMap, &keyLen);
	if(!success) {
		printf("Error ekey!\n");
		printf("%d", keyLen);
		return -1;
	}
	if(keyLen > 300)
		InitRC4KSA();

	char outName[128];
	strcpy(outName, argv[1]);
	set_ext(outName, ".flac");
	FILE *fin = fopen(argv[1], "rb");
	FILE *fout = fopen(outName, "wb");
	unsigned char buffer[BUFFER_SIZE];
	unsigned char xorMap[BUFFER_SIZE];

	int n, i;
	unsigned __int64 totalBytes = 0;
	while ((n = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
		if(keyLen <= 300)
			ProcessByMapL(totalBytes, xorMap, n);
		else
			ProcessByRC4(totalBytes, xorMap, n);

		for(i = 0; i < n; i++)
			buffer[i] ^= xorMap[i];
		fwrite(buffer,  n, 1, fout);

		totalBytes += n;
	}

	fclose(fin);
	fclose(fout);
	printf("Processed: %s\n", outName);
	return 0;
}

