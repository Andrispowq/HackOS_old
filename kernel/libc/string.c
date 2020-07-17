#include "string.h"

/**
 * K&R implementation
 */
void itoa(uint8_t* buf, uint32_t base, uint32_t d) 
{
    uint8_t* p = buf;
    uint8_t* p1, *p2;
    uint32_t ud = d;
    uint32_t divisor = 10;

    if(base == 'd' && d < 0) 
    {   
        *p++ = '-';
        buf++;
        ud = -d;
    }
    else
    {
        if (base == 'x')
        {
            divisor = 16;
        }
    }

    do
    {
        uint32_t remainder = ud % divisor;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
    }
    while (ud /= divisor);

    *p = 0;
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2) 
    {
        uint8_t tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

/* K&R */
void reverse(char s[]) 
{
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) 
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
int strlen(char s[]) 
{
    int i = 0;
    while (s[i] != '\0') 
        ++i;

    return i;
}

void append(char s[], char n) 
{
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

void backspace(char s[]) 
{
    int len = strlen(s);
    s[len - 1] = '\0';
}

/* K&R 
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(char s1[], char s2[]) 
{
    int i;
    for (i = 0; s1[i] == s2[i]; i++) 
    {
        if (s1[i] == '\0') 
        return 0;
    }

    return s1[i] - s2[i];
}

int strncmp(char s1[], char s2[], int bytes)
{
    int i;
    for (i = 0; s1[i] == s2[i] && i < bytes; i++) 
    {
        if (s1[i] == '\0') 
            return 0;
    }

    return s1[i] - s2[i];
}