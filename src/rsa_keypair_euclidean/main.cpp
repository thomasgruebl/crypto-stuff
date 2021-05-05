
// RSA generate public-private keypair with euclidean algorithm

// run using:
// g++ main.cpp -o run.o -lgmpxx -lgmp
// ./run.o


#include <iostream>
#include <gmpxx.h>

using namespace std;

int main(void) {

    mpz_class rand_Num;
    mpz_class p;
    mpz_class q;
    mpz_class N;
    mpz_class d;
    mpz_class e;
    mpz_class phiN;

    gmp_randstate_t r_state;
    mp_bitcnt_t half_key_len = 1024;

    //key length 2048 (1024+1024)

    gmp_randinit_default(r_state);

    //generate p and q; find next prime
    do {
        mpz_urandomb(rand_Num.get_mpz_t(), r_state, half_key_len);
        mpz_nextprime(p.get_mpz_t(), rand_Num.get_mpz_t());
        mpz_urandomb(rand_Num.get_mpz_t(), r_state, half_key_len);
        mpz_nextprime(q.get_mpz_t(), rand_Num.get_mpz_t());
    } while(p == q);

    //calculate N
    N = p * q;
    //calculate phi(N)
    phiN = (p-1) * (q-1);

    //find e
    for (e = 2; e!= phiN; e++) {
        mpz_class rop;
        mpz_gcd(rop.get_mpz_t(), e.get_mpz_t(), phiN.get_mpz_t());
        if (rop == 1) {
            //cout << e << endl;
            break;
        }
    }

    //calculate d using the Euclidean algorithm

    mpz_class a, b, u1, v1, tmp1, tmp3, g, i;

    a = e;
    b = phiN;
    u1 = 1;
    v1 = 0;
    i = 1;

    while(b != 0) {
        g = a / b;
        tmp3 = a % b;
        tmp1 = u1 + g * v1;
        u1 = v1;
        v1 = tmp1;
        a = b;
        b = tmp3;
        i = -i;
    }

    if(a != 1) {
        d = 0;
    }
    if(i < 0) {
        d = phiN - u1;
    }
    else {
        d = u1;
    }

    cout << "PUBLIC_KEY (e, N): " << endl << endl << "(" << e << ", " << N << ")" << endl << endl;
    cout << "PRIVATE_KEY (d, N): " << endl << endl << "(" << d << ", " << N << ")" << endl;

    return 0;
}
