
// Sign and verify signatures with libsodium

// run using:
// g++ main.cpp -o run.o -lgmpxx -lgmp -lsodium
// ./run.o Sign/Verify m d/e n

#include <iostream>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <sodium.h>
#include <gmpxx.h>

//#define DEBUG

using namespace std;


void HexStringToArray ( const char * const text , unsigned char * const array , const size_t array_size ) {
    for ( size_t i = 0; i < array_size ; i++) {
        const string text_part ( text + 2 * i, 2); // Process 2chars at a time
        stringstream s( text_part );
        s >> hex;
        int value ;
        s >> value ;
        array [i] = value ;
    }
}

// used libsodium documentation: https://download.libsodium.org/doc/public-key_cryptography/public-key_signatures.html

void sign(const char* MESSAGE, size_t MESSAGE_LEN) {
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];
    crypto_sign_keypair(pk, sk);

    unsigned char signed_message[crypto_sign_BYTES + MESSAGE_LEN];
    unsigned long long signed_message_len;

    crypto_sign(signed_message, &signed_message_len, (const unsigned char*)MESSAGE, MESSAGE_LEN, sk);


    cout << "Signed Message: " << hex;
    for (size_t i = 0; i < sizeof signed_message; i++) {
        cout << setw(2) << setfill('0') << (int)signed_message[i];
    }
    cout << endl << "Public Key: " << hex;

    for (size_t i = 0; i < sizeof pk; i++) {
        cout << setw(2) << setfill('0') << (int)pk[i];
    }
    cout << endl;
}

void verify(const char* MESSAGE, size_t MESSAGE_LEN, const char* SIGNATURE, const char* p_k, size_t p_k_size) {
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_hash_sha512_BYTES];

    HexStringToArray(p_k, pk, p_k_size);
    HexStringToArray(SIGNATURE, sk, crypto_hash_sha512_BYTES);

    if (crypto_sign_verify_detached(sk, (const unsigned char *)MESSAGE, MESSAGE_LEN, pk) != 0) {
        cout <<"Signature invalid." << endl;
    }
    else {
        cout <<"Signature valid." << endl;
    }
}

int main(const int argc, const char * const argv[]) {
    if (sodium_init() == -1) {
        return 1;
    } else if (argc < 3) {
        cerr << "You need more arguments!" << endl;
        return 1;
    }

    const char *MESSAGE = argv[2];
    const char *SIGNATURE = argv[3];
    const char *p_k = argv[4];
    size_t p_k_size = strlen(p_k);
    size_t MESSAGE_LEN = strlen(MESSAGE);

    if (strcmp(argv[1], "Sign") == 0) {
        sign(MESSAGE, MESSAGE_LEN);
    } else if (strcmp(argv[1], "Verify") == 0) {
        verify(MESSAGE, MESSAGE_LEN, SIGNATURE, p_k, p_k_size);
    }

    return 0;
}