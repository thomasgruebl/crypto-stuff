
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

// Source: https://www.andreas-unterweger.eu/wp-content/uploads/Aufgaben_Signaturen.pdf
void libsodium_to_GMP (const unsigned char (& libsodium_value)[crypto_hash_sha512_BYTES], mpz_class & GMP_value) {
    stringstream s;
    s << hex;
    for (size_t i = 0; i < sizeof libsodium_value ; i++)
        s << setw (2) << setfill ('0') << (int) libsodium_value[i];
    const char * const string_as_hex = s.str().c_str();
    mpz_set_str (GMP_value.get_mpz_t (), string_as_hex , 16);
}

int main(const int argc, const char * const argv[]) {
    if (sodium_init() == -1)
        return 1;

    if ((argc < 5) || (argc > 6)) {
        cout << "Wrong usage!" << endl;
        return 1;
    }

    string mode(argv[1]);
    mpz_class hValue;
    const char * const message = argv[2];
    const size_t message_length = strlen(message);
    unsigned char hash[crypto_hash_sha512_BYTES];
    crypto_hash_sha512(hash, (const unsigned char*)message, message_length);

    if (mode == "Sign") {
        libsodium_to_GMP(hash, hValue);
        const mpz_class exp(argv[3]), mod(argv[4]);
        mpz_class rop;
        mpz_powm(rop.get_mpz_t(), hValue.get_mpz_t(), exp.get_mpz_t(), mod.get_mpz_t());
        cout << rop.get_mpz_t() << endl;
    }
    else if (mode == "Verify") {
        libsodium_to_GMP(hash, hValue);
        const mpz_class m(argv[3]), exp(argv[4]), mod(argv[5]);
        mpz_class rop;
        mpz_powm(rop.get_mpz_t(), m.get_mpz_t(), exp.get_mpz_t(), mod.get_mpz_t());

        if (rop == hValue) {
            cout << "Signature valid." << endl;
        }
        else {
            cout << "Signature invalid." << endl;
        }

#ifdef DEBUG
        cout << rop.get_mpz_t() << endl;
		cout << hValue.get_mpz_t() << endl;
#endif

    }
    else {
        cout << "Wrong usage!" << endl;
        return 1;
    }

    return 0;
}