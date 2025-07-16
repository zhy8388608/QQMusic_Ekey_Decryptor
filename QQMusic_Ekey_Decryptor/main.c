#include "common.h"

#ifdef _WIN32
	#include <windows.h>
#endif

#define BUFFER_SIZE 8192

unsigned char keyMap[1024];
unsigned int keyLen = 1024;

char *set_ext(char *filename, char *new_ext) {
	char *dot = strrchr(filename, '.');
	if(dot)strcpy(dot, new_ext);
	else strcat(filename, new_ext);
	return filename;
}

int main(int argc, char *argv[]) {
	printf("\n");

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

	char tmpName[256];
	char outName[256];

	if(argc == 3)
		strcpy(tmpName, argv[2]);
	else
		strcpy(tmpName, argv[3]);
	set_ext(tmpName, ".tmp");

	FILE *fin = fopen(argv[2], "rb");
	if(!fin) {
		printf("Error opening input file: %s\n", argv[2]);
		return -1;
	}
	FILE *fout = fopen(tmpName, "wb");
	if(!fout) {
		printf("Error opening output file: %s\n", tmpName);
		fclose(fin);
		return -1;
	}

	unsigned char buffer[BUFFER_SIZE];
	unsigned __int64 totalBytes = 0;
	int n, i;
	while((n = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
		if(keyLen <= 300)
			ProcessByMapL(totalBytes, buffer, n);
		else
			ProcessByRC4(totalBytes, buffer, n);
		fwrite(buffer, n, 1, fout);
		totalBytes += n;
	}

	fclose(fin);
	fclose(fout);

	if(argc == 3) {
		strcpy(outName, argv[2]);
		set_ext(outName, ".bin");
		FILE *fin1 = fopen(tmpName, "rb");
		if(fread(buffer, 1, 5, fin1) == 5)
			if(strcmp(buffer, "fLaC") == 0)
				set_ext(outName, ".flac");
			else if(strcmp(buffer, "OggS") == 0)
				set_ext(outName, ".ogg");
		fclose(fin1);
	} else 
		strcpy(outName, argv[3]);

	#ifdef _WIN32
		MoveFileEx(tmpName, outName, MOVEFILE_REPLACE_EXISTING);
	#else
		rename(tmpName, outName);
	#endif

	printf("Processed: %s\n", outName);
	return 0;
}

