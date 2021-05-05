
// Simple RSA encrypt/decrypt

// run using:
// g++ main.cpp -o run.o -lgmpxx -lgmp
// ./run.o m e N


#include <iostream>
#include <gmpxx.h>

using namespace std;

int main(const int argc, const char * const argv[]) {
    if (argc < 4) {
        cerr << "Description: Adds m, e and N" << endl;
        cerr << "Usage: " << argv[0] << " <a> <b>" << endl;
        cerr << "  a: First number to add" << endl;
        cerr << "  b: Second number to add" << endl;
        cerr << "  c: Third number to add" << endl;
        return 1;
    }

    mpz_class m(argv[1]);    //m = 7
    mpz_class e_d(argv[2]);  //e = 29   d = 85
    mpz_class N(argv[3]);    //N = 391
    mpz_class c;

    mpz_powm(c.get_mpz_t(), m.get_mpz_t(), e_d.get_mpz_t(), N.get_mpz_t());
    cout << "Output: " << c << endl;

    return 0;
}
