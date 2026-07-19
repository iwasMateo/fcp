#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include "user-code.h"

#define ENCODED_LENGTH 26
#define GROUPED_LENGTH 33  // 26 chars + 6 hyphens + 1 null terminator

const char ALPHABET[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

// Helper to look up the value of a Crockford Base32 character
int get_alphabet_value(char c) {
    for (int i = 0; i < 32; i++) {
        if (ALPHABET[i] == c) {
            return i;
        }
    }
    return -1; // Invalid character
}

// Convert IPv6 string to a human-readable Crockford Base32 string
int encode_ipv6(const char *ipv6_str, char *output) {
    struct in6_addr addr;
    
    // Parse the IPv6 string into bytes
    if (inet_pton(AF_INET6, ipv6_str, &addr) != 1) {
        return 0; // Invalid IPv6 address
    }

    // Load the 16 bytes into a 128-bit unsigned integer
    unsigned __int128 value = 0;
    for (int i = 0; i < 16; i++) {
        value = (value << 8) | addr.s6_addr[i];
    }

    // Temporary buffer to hold the raw 26 base32 characters
    char temp[ENCODED_LENGTH + 1];
    memset(temp, '0', ENCODED_LENGTH);
    temp[ENCODED_LENGTH] = '\0';

    // Extract base32 characters from right to left
    int idx = ENCODED_LENGTH - 1;
    while (value > 0 && idx >= 0) {
        temp[idx--] = ALPHABET[value % 32];
        value /= 32;
    }

    // Format into the output buffer with hyphens every 4 characters
    int out_idx = 0;
    for (int i = 0; i < ENCODED_LENGTH; i++) {
        if (i > 0 && i % 4 == 0) {
            output[out_idx++] = '-';
        }
        output[out_idx++] = temp[i];
    }
    output[out_idx] = '\0';

    return 1; // Success
}

// Convert a Crockford Base32 string back into an IPv6 address string
int decode_ipv6(const char *encoded, char *output_ipv6) {
    unsigned __int128 value = 0;

    // Reconstruct the 128-bit integer from the base32 string
    for (int i = 0; encoded[i] != '\0'; i++) {
        char c = encoded[i];
        if (c == '-') continue; // Skip hyphens
        
        c = toupper((unsigned char)c);
        int val = get_alphabet_value(c);
        if (val == -1) {
            return 0; // Invalid base32 character encountered
        }

        value = (value * 32) + val;
    }

    // Extract the 128-bit integer back into 16 bytes
    struct in6_addr addr;
    for (int i = 15; i >= 0; i--) {
        addr.s6_addr[i] = value & 0xFF;
        value >>= 8;
    }

    // Convert the bytes back to a standard IPv6 text representation
    if (inet_ntop(AF_INET6, &addr, output_ipv6, INET6_ADDRSTRLEN) == NULL) {
        return 0;
    }

    return 1; // Success
}
