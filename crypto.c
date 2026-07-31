#include <sodium.h>
#include <stdio.h>

int make_keypair(unsigned char *public_key[crypto_sign_PUBLICKEYBYTES], unsigned char *secret_key[crypto_sign_SECRETKEYBYTES]) {
    crypto_sign_keypair(*public_key, *secret_key);
}
int get_keypair() {
    FILE* public_key = fopen("public_key.txt", "r+");
    if (public_key == NULL) {
        return -1;
    }
    FILE* private_key = fopen("private_key,txt", "r+");
    if (private_key == NULL) {
        return -1;
    }
}