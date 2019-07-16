/* my-grep.c */
/* Juuso Ylikoski 0526228 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void etsinta(char *sana, char *filename) {
	FILE *tiedosto;
	size_t n = 0;
	ssize_t rivi_pituus;
	char* rivi;
	if (strcmp(filename, "stdin") == 0) {
		while ((rivi_pituus = getline(&rivi, &n, stdin)) >= 0) {
			if (strstr(rivi, sana) != NULL) {
				printf("%s",rivi); } }
	} else {
		if ((tiedosto = fopen(filename, "r")) == NULL) {
			perror("my-grep: cannot open file\n");
			exit(1); }
		while ((rivi_pituus = getline(&rivi, &n, tiedosto)) >= 0) {
			if (strstr(rivi, sana) != NULL) {
				printf("%s",rivi); } }
	}
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("my-grep: searchterm [file ...]\n");
		exit(1);
	} else if (argc > 2) {
		for (int i = 2; i < argc; i++) {
			etsinta(argv[1], argv[i]);
		}
	} else {
		etsinta(argv[1], "stdin");}
	return 0;
}
