/* my-zip.c */
/* Juuso Ylikoski 0526228 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tulosta(char *filename) {
	FILE *tiedosto;
	size_t n = 0;
	ssize_t rivi_pituus;
	char* rivi;
	char akirjain, bkirjain;
	int k_numero = 0, maara = 1;
	if ((tiedosto = fopen(filename, "r")) == NULL) {
		perror("my-zip: cannot open file\n");
		exit(1); }
	while ((rivi_pituus = getline(&rivi, &n, tiedosto)) >= 0) {
		k_numero = 0;
		akirjain = rivi[k_numero];
		while (akirjain != '\0') {
			//Pick next char
			bkirjain = rivi[k_numero + 1];
			//If it's the same one, add to the amount, else write current amount and char
			if (akirjain == bkirjain) {
				maara++;
			} else {
				fwrite(&maara,4,1,stdout);
				fwrite(&akirjain, 1, 1, stdout);
				maara = 1; }
			k_numero++;
			akirjain = rivi[k_numero];
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
