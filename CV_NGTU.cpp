#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <omp.h>

using namespace std;

typedef map<string, int> WordFrequencyMap;

void sanitizeWord(string& word) {
    word.erase(remove_if(word.begin(), word.end(), [](char c) { return !isalnum(c); }), word.end());
}

void processFile(ifstream& file, WordFrequencyMap& wordFreqMap) {
    string word;
    while (file >> word) {
        sanitizeWord(word);
#pragma omp critical
        ++wordFreqMap[word];
    }
}

bool sortByFrequency(const pair<string, int>& a, const pair<string, int>& b) {
    return a.second > b.second;
}

int main() {
    string filename = "text.txt";
    WordFrequencyMap wordFrequencyMap;

#pragma omp parallel
    {
#pragma omp single nowait
        {
            ifstream inFile(filename);
            if (!inFile.is_open()) {
                cout << "Failed to open file." << endl;
            }
            processFile(inFile, wordFrequencyMap);
            inFile.close();
        }
    }

    vector<pair<string, int>> sortedWordFreq(wordFrequencyMap.begin(), wordFrequencyMap.end());
    sort(sortedWordFreq.begin(), sortedWordFreq.end(), sortByFrequency);

    for (const auto& pair : sortedWordFreq) {
        cout << pair.first << " : " << pair.second << " occurrences" << endl;
    }

    return 0;
}