#include "userlib.h"
#include <stdint.h>
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);


void unsigned_numtostr(unsigned int num, char *str) {
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void signed_numtostr(int num, char *str) {
    if (num < 0) {
        *str++ = '-';
        num = -num;
    }
    unsigned_numtostr((unsigned int)num, str);
}

void strToMinus(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += ('a' - 'A');
    }
}

void unsigned_numtohex(unsigned int num, char *str) {
    const char hexDigits[] = "0123456789ABCDEF";
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num > 0) {
        str[i++] = hexDigits[num % 16];
        num /= 16;
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void strToMayus(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= ('a' - 'A');
        }
    }
}


int strcmp(const char *s1, const char *s2)
{
    int i, toReturn = 0, checked = 0;
    for (i = 0; s1[i] && s2[i]; i++)
    {
        if (!checked)
        {
            toReturn += (s1[i] - s2[i]);
            checked = toReturn == 0 ? 0 : 1;
        }
    }
    if (s1[i])
    {
        toReturn = s1[i];
    }
    else if (s2[i])
    {
        toReturn = s2[i];
    }
    return toReturn;
}


  char global_buffer[2];
  void putChar(char c) {
      global_buffer[0] = c;
      syscall(1, 1, (uint64_t)global_buffer, 1);
  }
