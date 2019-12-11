/* my-unzip.c */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tulosta(char *filename) {
	FILE *tiedosto;
	char letter;
	int value;
	if ((tiedosto = fopen(filename, "r")) == NULL) {
		perror("my-unzip: cannot open file\n");
		exit(1); }
	//swicth between reading a 4byte integer and 1byte char
	while(fread(&value,4,1,tiedosto) != 0) {
		fread(&letter,1,1,tiedosto);
		for (int i=0; i < value; i++) {
				printf("%c",letter);
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("my-zip: file1 [file2 ...]\n");
		exit(1);
	} else {
	for (int i=1; i < argc; i++) {
		tulosta(argv[i]); }
}
	return 0;
}
