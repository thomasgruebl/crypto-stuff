// Caesar decipher (Caps only)

// run using:
// g++ main.cpp -o run.o
// ./run.o inputfile.txt outputfile.txt key

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[]) {

    ifstream inputFile;
    ofstream outputFile;

    int key;
    char c;
    char tmp;
    vector<char> ciphertext;
    vector<char>::iterator pos;

    if (argc != 4) {
        cout << "Input filepath, output filepath and key required!" << endl;
        return 1;
    }

    inputFile.open(argv[1]);
    outputFile.open(argv[2]);
    key = atoi(argv[3]);

    if ((key < 0) || (key > 25)) {
        cout << "Key length must be between 1 and 25!" << endl;
        cout << "Retry!" << endl;
        return 1;
    }
    else if ((!inputFile) || (!outputFile)) {
        cout << "File reading error" << endl;
        return 1;
    }

    //disable white space skipping
    inputFile >> noskipws;
    while (inputFile) {
        inputFile >> c;
        ciphertext.push_back(c);
    }

    //decrypt ciphertext
    for (unsigned int i = 0; i != ciphertext.size(); i++) {
        if ((ciphertext.at(i) >= 65) && (ciphertext.at(i) <= 90)) {
            if((ciphertext.at(i) - key) < 65) {
                tmp = ciphertext.at(i) - key;
                tmp += 26;
                ciphertext.at(i) = tmp;
            }
            else {
                ciphertext.at(i) -= key;
            }
        }
    }

    for (pos = ciphertext.begin(); pos != ciphertext.end()- 1; pos++) {
        outputFile << *pos;
    }
    cout << "Plain text successfully copied to output file!" << endl;

    inputFile.close();
    outputFile.close();
}