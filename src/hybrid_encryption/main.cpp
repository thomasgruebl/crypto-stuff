
// Hybrid Encryption (Diffie-Hellman Key Exchange + AES)

// run using:
// g++ main.cpp -o run.o -lgmpxx -lgmp -lsodium
// ./run.o ServerGeneratePartialKey             -> Output: x and g^x
// ./run.o ClientGeneratePartialKey             -> Output: y and g^y
// ./run.o ServerGenerateSessionKey g^y x       -> Output: session_key in HEX
// ./run.o ClientGenerateSessionKey g^x y       -> Output: session_key in HEX (both session keys must be identical)
// ./run.o ServerEncrypt Message session_key    -> Output: Encrypted Message (enc_msg) in HEX
// ./run.o ClientDecrypt enc_msg session_key    -> Output: Decrypted Message in plain text


#include <iostream>
#include <sstream>
#include <string.h>
#include <iomanip>
#include <sodium.h>
#include <gmpxx.h>


using namespace std;

int message_len = 0;

string ExtractKey (const mpz_class & g_xy ) {
    mpz_class two_256 ;
    mpz_ui_pow_ui (two_256.get_mpz_t(), 2, 256) ;
    mpz_class g_xy_short;
    mpz_mod(g_xy_short.get_mpz_t() ,g_xy.get_mpz_t(), two_256.get_mpz_t());
    stringstream s;
    s << hex << g_xy_short << endl ;
    return s.str();
}

void HexStringToArray (const char * const text, unsigned char * const array, const size_t array_size ) {
    for (size_t i = 0; i < array_size ; i++) {
        const string text_part (text + 2 * i, 2); // Process 2chars at a time
        stringstream s(text_part);
        s >> hex;
        int value;
        s >> value;
        array[i] = value ;
    }
}

void GeneratePartialKey(const mpz_class g, const mpz_class p, const char* method) {
    gmp_randstate_t r_state;
    gmp_randinit_default(r_state);
    gmp_randseed_ui(r_state, time(NULL));
    mp_bitcnt_t len = 1024;

    mpz_class x_y, S_C;

    do {
        mpz_urandomb(x_y.get_mpz_t(), r_state, len);
    } while (x_y < 0 || x_y > (p-1));

    mpz_powm(S_C.get_mpz_t(), g.get_mpz_t(), x_y.get_mpz_t(), p.get_mpz_t());

    if (strcmp(method, "server") == 0) {
        cout << "x: " << x_y << endl;
        cout << "g^x: " << S_C << endl;
        //gxy->at(1) = x_y;
    }
    else if (strcmp(method, "client") == 0) {
        cout << "y: " << x_y << endl;
        cout << "g^y: " << S_C << endl;
        //gxy->at(2) = x_y;
    }
    else {
        cerr << "Wrong usage!" << endl;
    }

    //return *gxy;
}

void GenerateSessionKey(const mpz_class S_C, const mpz_class x_y, const mpz_class p) {
    mpz_class k;
    mpz_powm(k.get_mpz_t(), S_C.get_mpz_t(), x_y.get_mpz_t(), p.get_mpz_t());
    cout << hex <<  ExtractKey(k) << endl;
}

void ServerEncrypt(const char* message, const char* k) {
    message_len = strlen(message);

    //cout << "MESSAGE:" << message << endl;

    //source: https://download.libsodium.org/doc/secret-key_cryptography/aes-256-gcm.html

    const unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES] = {0};
    unsigned char key[crypto_aead_aes256gcm_KEYBYTES];

    HexStringToArray(k, key, crypto_aead_aes256gcm_KEYBYTES);

    //cout << "K:" << k << endl;
    unsigned char ciphertext[message_len + crypto_aead_aes256gcm_ABYTES];
    unsigned long long ciphertext_len;

    if (sodium_init() == -1) {
        cerr << "Init failed!" << endl;
    }

    if (crypto_aead_aes256gcm_is_available() == 0) {
        abort(); /* Not available on this CPU */
    }

    crypto_aead_aes256gcm_encrypt(ciphertext, &ciphertext_len, (const unsigned char*)message, message_len, NULL, 0, NULL, nonce, key);
    cout << hex;
    for (size_t i = 0; i < sizeof ciphertext; i++) {
        cout << setw(2) << setfill('0') << (int)ciphertext[i];
    }
    cout << endl;
}

void ClientDecrypt(const char* ciphertext, const char* k) {
    const unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES] = {0};
    unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
    unsigned long long decrypted_len;
    size_t ciphertext_len = strlen(ciphertext)/2;
    HexStringToArray(k, key, crypto_aead_aes256gcm_KEYBYTES);

    unsigned char cipher[ciphertext_len];
    HexStringToArray(ciphertext, cipher, ciphertext_len);
    int cipher_len = sizeof cipher;
    unsigned char decrypted[cipher_len];


    if(crypto_aead_aes256gcm_decrypt(decrypted, &decrypted_len, NULL, cipher, ciphertext_len, NULL, 0, nonce, key) != 0) {
        cout << "Message forged" << endl;
    }
    else {
        cout << decrypted << endl;
    }
}

int main(const int argc, const char * const argv[]) {

    if ((argc < 1) || (argc > 4)) {
        cerr << "Wrong usage!" << endl;
        return 1;
    }
    else if (sodium_init() == -1) {
        return 1;
    }

    //used following source: https: //docs.oracle.com/javase/7/docs/technotes/guides/security/StandardNames.html

    const char * const g_as_text = "\
	f7e1a085d69b3ddecbbcab5c36b857b97994afbbfa3aea82f \
	9574c0b3d0782675159578ebad4594fe67107108180b44916 \
	7123e84c281613b7cf09328cc8a6e13c167a8b547c8d28e0a \
	3ae1e2bb3a675916ea37f0bfa213562f1fb627a01243bcca4 \
	f1bea8519089a883dfe15ae59f06928b665e807b552564014 \
	c3bfecf492a";

    const char * const p_as_text = "\
	fd7f53811d75122952df4a9c2eece4e7f611b7523cef4400c \
	31e3f80b6512669455d402251fb593d8d58fabfc5f5ba30f6 \
	cb9b556cd7813b801d346ff26660b76b9950a5a49f9fe8047 \
	b1022c24fbba9d7feb7c61bf83b57e7c6a8a6150f04fb83f6 \
	d3c51ec3023554135a169132f675f3ae2b61d72aeff222031 \
	99dd14801c7";

    const mpz_class g(g_as_text, 16);
    const mpz_class p(p_as_text, 16);

    if (strcmp(argv[1], "ServerGeneratePartialKey") == 0) {
        GeneratePartialKey(g, p, "server");
    }
    else if (strcmp(argv[1], "ClientGeneratePartialKey") == 0) {
        GeneratePartialKey(g, p, "client");
    }
    else if (strcmp(argv[1], "ServerGenerateSessionKey") == 0) {
        mpz_class C(argv[2]);
        mpz_class x(argv[3]);
        GenerateSessionKey(C, x, p);
    }
    else if (strcmp(argv[1], "ClientGenerateSessionKey") == 0) {
        mpz_class S(argv[2]);
        mpz_class y(argv[3]);
        GenerateSessionKey(S, y, p);
    }
    else if (strcmp(argv[1], "ServerEncrypt") == 0) {
        const char* message(argv[2]);
        const char* k(argv[3]);
        ServerEncrypt(message, k);
    }
    else if (strcmp(argv[1], "ClientDecrypt") == 0) {
        const char* ciphertext(argv[2]);
        const char* k(argv[3]);
        ClientDecrypt(ciphertext, k);
    }
    else {
        cerr << "Wrong argv arguments!" << endl;
    }

    return 0;
}