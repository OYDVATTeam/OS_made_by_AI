#include "stdio.h"
#include "string.h"

/**
 * itoa: Convert integer to string.
 * @n: The number to convert
 * @s: The buffer to store the string
 * @base: The base (10 for decimal, 16 for hex)
 */
void itoa(int n, char* s, int base) {
    int i = 0;
    int sign = n;

    // Handle negative numbers for base 10
    if (sign < 0 && base == 10) {
        n = -n;
    }

    // Convert digits in reverse order
    do {
        int digit = n % base;
        s[i++] = (digit > 9) ? (digit - 10) + 'A' : digit + '0';
    } while ((n /= base) > 0);

    // Add negative sign
    if (sign < 0 && base == 10) {
        s[i++] = '-';
    }

    s[i] = '\0';

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = s[start];
        s[start] = s[end];
        s[end] = temp;
        start++;
        end--;
    }
}

/**
 * A very basic helper to format a string with one integer.
 * Useful for GUI labels like: format_label(buf, "X: ", mx);
 */
void format_label(char* buf, const char* prefix, int val) {
    char num_buf[12];
    itoa(val, num_buf, 10);
    
    // Copy prefix
    int i = 0;
    while (prefix[i]) {
        buf[i] = prefix[i];
        i++;
    }
    
    // Copy number
    int j = 0;
    while (num_buf[j]) {
        buf[i++] = num_buf[j++];
    }
    buf[i] = '\0';
}
