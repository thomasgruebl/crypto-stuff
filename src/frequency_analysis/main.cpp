// Frequency Analysis

// run using:
// g++ main.cpp -o run.o
// ./run.o inputfile.txt

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>


using namespace std;

typedef struct REL {
    double rel;
    char c;
}Rel;

//compare function
bool cmp(int i, int j) {
    return i < j;
}

//sort array from largest number to smallest
void sort_frequencies(Rel R[]) {
    int i;
    bool tmp = true;

    while(tmp){
        tmp = false;
        for (i = 0; i < 25; i++) {
            if (R[i].rel < R[i+1].rel) {
                swap(R[i], R[i+1]);
                tmp = true;
            }
        }
    }

}

//count absolute frequencies of letters
vector<int> frequency_count(vector<char> letters, vector<int> frequencies) {
    int ascii;
    int i;
    vector<char>::iterator pos;

    for (pos = letters.begin(); pos != letters.end(); pos++) {
        for (ascii = 65; ascii < 122; ascii++) {
            if (*pos == ascii) {
                i = ascii - 'A';
                frequencies.at(i)++;
            }
        }
    }
    frequencies.at(i)--;

    return frequencies;
}

int main(int argc, char* argv[]) {

    ifstream inputFile;
    inputFile.open(argv[1]);

    if (argc != 2) {
        cout << "Please add only the filepath argument!" << endl;
        return 1;
    }

    if(!inputFile) {
        cout << "File reading error" << endl;
        return 1;
    }

    char c;
    int len = 0;
    Rel R[26];
    vector<char> letters;
    vector<int> frequencies (58);
    vector<char>::iterator pos;


    while (inputFile) {
        inputFile >> c;
        letters.push_back(c);
    }

    sort(letters.begin(), letters.end(), cmp);

    frequencies = frequency_count(letters, frequencies);

    for(pos = letters.begin(); pos != letters.end(); pos++) {
        if ((*pos >= 'A') && (*pos <= 'Z')) {
            len++;
        }
    }

    //copy relative frequencies into array
    for (int i = 0; i < 26; i++) {
        R[i].rel = (double(frequencies.at(i)) + double(frequencies.at(i+32))) / len;
        R[i].c = 65+i;
    }

    sort_frequencies(R);

    //print relative frequencies
    for (auto & i : R) {
        if ((i.rel == 0) || (i.c == ' ')) {
            continue;
        }
        else {
            cout << i.c << ":  " << fixed << setprecision(0) << i.rel *100 << "%" << endl;
        }
    }


    inputFile.close();
    return 0;
}
