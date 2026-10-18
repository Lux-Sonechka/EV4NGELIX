#include "string.h"

int strlen (char str[])
{
	int count = 0;
	while(str[count] != '\0') {
		count++;
	}
	return count;
}
int char_to_hex(const char* str) {
    if (!str || !str[0]) return -1;  // erro: string vazia
    return (unsigned char)str[0];    // retorna o código ASCII
}
void encode_to_hex(const char* input, char* output, int length) {
    while (length--) {
        unsigned char c = *input++;
        *output++ = "0123456789ABCDEF"[c >> 4];
        *output++ = "0123456789ABCDEF"[c & 0x0F];
    }
    *output = '\0';
}
int hex_char_to_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

void decode_from_hex(const char* hex, char* output) {
    while (*hex && hex[1]) {
        int high = hex_char_to_val(*hex++);
        int low  = hex_char_to_val(*hex++);
        *output++ = (high << 4) | low;
    }
    *output = '\0';
}


void reverse (char str[])
{
	int temp, i, j;
	for (i = 0, j = strlen(str) - 1; i < j; i++, j--) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
	}
}

void int_to_ascii (int n, char str[]) 
{
	int i, sign;
	if((sign=n) < 0) n = -n;
	i = 0;
	do {
		str[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if(sign < 0) str[i++] = '-';
	str[i] = '\0';

	reverse(str);
}

void hex_to_ascii (int n, char str[])
{
	int i, sign;
	if((sign=n) < 0) n = -n;
	i = 0;
	do {
		if (n % 16 <= 9) {
			str[i++] = n % 16 + '0';
		} else {
			str[i++] = n % 16 + 'A';
		}
	} while ((n /= 16) > 0);

	if(sign < 0) str[i++] = '-';
	str[i] = '\0';

	reverse(str);
}

void backspace (char str[])
{
	int len = strlen(str);
	str[len-1] = '\0';
}

void append (char str[], char letter)
{
	int len = strlen(str);
	str[len] = letter;
	str[len+1] = '\0';
}

int strcmp (char s1[], char s2[])
{
	int i;
	for (i=0; s1[i] == s2[i]; i++)
		if (s1[i] == '\0')
			return 0;
	return s1[i] - s2[i];
}
char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == (char)c) {
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

char* strtok(char* str, const char* delim) {
    static char* p = NULL;
    if (str) p = str;

    if (!p || *p == '\0') return NULL;

    // Pula delimitadores iniciais
    while (*p && strchr(delim, *p)) p++;

    if (*p == '\0') return NULL;

    char* start = p;

    // Avança até o próximo delimitador ou fim da string
    while (*p && !strchr(delim, *p)) p++;

    if (*p) {
        *p = '\0';
        p++;
    }

    return start;
}
char *strncpy(char *dest, const char *src, unsigned int n) {
    unsigned int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    // Preenche o resto com '\0' se src for menor que n
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}
int toupper(int ch) {
    if (ch >= 'a' && ch <= 'z') {
        return ch - 32;
    }
    return ch;
}
int tolower(int ch) {
	if (ch >= 'A' && ch <= 'Z') {
		return ch + 32;
	}
	return ch;
}