#include "common.h"

#define BUFFER_SIZE 8192

unsigned char keyMap[1024];
unsigned int keyLen = 1024;

void set_ext(char *filename, const char *new_ext) {
	char *dot = strrchr(filename, '.');
	if (dot) strcpy(dot , new_ext);
	else strcat(filename, new_ext);
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf("Usage: %s <ekey_base64> <file_in> [<file_out>]\n", argv[0]);
		return -1;
	}
	int valid = decryptEkey(argv[1], strlen(argv[1]), keyMap, &keyLen);
	if(!valid) {
		printf("Error ekey!\n");
		return -1;
	}
	if(keyLen > 300)
		InitRC4KSA();

	char outName[128];
	if(argc == 3) {
		strcpy(outName, argv[2]);
		set_ext(outName, ".flac");
	} else
		strcpy(outName, argv[3]);

	FILE *fin = fopen(argv[2], "rb");
	if (!fin) {
		printf("Error opening input file: %s", argv[2]);
		return -1;
	}
	FILE *fout = fopen(outName, "wb");
	if (!fout) {
		printf("Error opening output file: %s", outName);
		fclose(fin);
		return -1;
	}

	unsigned char buffer[BUFFER_SIZE];
	unsigned char xorMap[BUFFER_SIZE];
	unsigned __int64 totalBytes = 0;
	int n, i;
	while ((n = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
		if(keyLen <= 300)
			ProcessByMapL(totalBytes, xorMap, n);
		else
			ProcessByRC4(totalBytes, xorMap, n);

		for(i = 0; i < n; i++)
			buffer[i] ^= xorMap[i];
		fwrite(buffer, n, 1, fout);

		totalBytes += n;
	}

	fclose(fin);
	fclose(fout);
	printf("\nProcessed: %s\n", outName);
	return 0;
}

