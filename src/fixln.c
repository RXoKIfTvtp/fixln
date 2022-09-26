
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Converts a line terminator from the Windows "\r\n" to the
 * linux line terminator "\n"
 * Removes Carriage Returns if it is followed by a Line Feed.
 * Returns the new length of the str string
**/
int cleanr(char *str, int len) {
	char *src = str;
	char *dst = str;
	int i;
	for (i = 0; i < len; i++) {
		if (*src == '\r' && ((i + 1) < len && *(src + 1) == '\n')) {
			src++;
		} else {
			*dst++ = *src++;
		}
	}
	return dst - str;
}

/**
 * Appends .tmp to a given string to create a temporary file name.
 * The temporary file name is allocated on the heap.
 * Returns a pointer to the temporary file name on the heap.
**/
char *mktname(char *str) {
	char ext[] = ".tmp";
	char *tmp = (char *)malloc(strlen(str) + strlen(ext) + 1);
	strcpy(tmp, str);
	strcpy(tmp + strlen(str), ext);
    // strlen doesn't account for the null terminator
    // copy it over to maintain C string structure
	tmp[strlen(str) + strlen(ext)] = 0;
	return tmp;
}

/**
 * Changes all Windows line terminators to Linux terminators
 * using a temporary file. Once the original has been fully
 * checked and copied to the temp file. the original is deleted
 * and the tmp is renamed to the original.
**/
void fix(char *fname) {
	unsigned char *tname = mktname(fname);
	FILE *ffp = NULL;
	FILE *tfp = NULL;
	
	if ((ffp = fopen(fname, "rb")) != NULL && (tfp = fopen(tname, "wb")) != NULL) {
		unsigned char buffer[4096];
		size_t read;
		
		while ((read = fread(buffer, 1, sizeof(buffer), ffp)) > 0) {
			read = cleanr(buffer, read);
			fwrite(buffer, 1, read, tfp);
		}
		
		fclose(ffp);
		fclose(tfp);
		
		remove(fname);
		rename(tname, fname);
	} else if (ffp == NULL) {
		printf("Could not open %s.\n", fname);
	} else if (tfp == NULL) {
		printf("Could not create temporary file %s.\n", tname);
	}
	free(tname); // Free temp file name from heap
}

int main(int argc, char *argv[]) {
	int i;
	for (i = 1; i < argc; i++) {
		fix(argv[i]);
	}
	return 0;
}
