/* my-cat.c */
/* Juuso Ylikoski 0526228 */

#include <stdio.h>
#include <stdlib.h>

void tulostus(char *filename) {
	char rivi[60];
	FILE *tiedosto;
	if ((tiedosto = fopen(filename, "r")) == NULL) {
		perror("my-cat: cannot open file\n");
		exit(1); }
	while (fgets(rivi, 60, tiedosto)) {
	printf("%s",rivi); }
	fclose(tiedosto);
}

int main(int argc, char *argv[]) {
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			tulostus(argv[i]);
		}
	}
	return 0;
}
