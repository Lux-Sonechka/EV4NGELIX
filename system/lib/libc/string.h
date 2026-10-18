#ifndef STRINGS_H
#define STRINGS_H

#define NULL ((void*)0)

void int_to_ascii (int n, char str[]); 
void hex_to_ascii (int n, char str[]);
int strcmp (char s1[], char s2[]);
int char_to_hex (const char* str);
char *strncpy(char *dest, const char *src, unsigned int n);
int toupper(int ch);
int tolower(int ch);

#endif
