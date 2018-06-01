#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

using namespace std;

vector<string> parseCSVLine(const string &line);
void writeCSVToXML(const string &airportFile, const string &airlineFile, const string &routeFile, const string &outputFile);
unordered_map<int, string> makeIdToNameMap(const string &airlineFile);

int main() {
    writeCSVToXML("airports.dat", "airlines.dat", "routes.dat", "flights.xml");
    return 0;
}

void writeCSVToXML(const string &airportFile, const string &airlineFile, const string &routeFile, const string &outputFile) {

    ifstream reader(airportFile.c_str());
    string line;
    unordered_map<int, string> airlineNames;

    airlineNames = makeIdToNameMap(airlineFile);

    while(getline(reader,line)) {
//        cout << line << endl;
        vector<string> lines = parseCSVLine(line);
//        for(size_t i = 0; i < lines.size(); ++i) {
//            cout << i << " : " << lines[i] << endl;
//        }
//        cout << endl;
    }
}

// Generates an unordered map with (airline id, airline name) pairs
// This is for easy dictionary lookup when writing out to xml
unordered_map<int, string> makeIdToNameMap(const string &airlineFile) {
    ifstream infile(airlineFile.c_str());
    string line;
    unordered_map<int, string> temp;

    // Seperates each line into vectors, and puts needed values in map
    while(getline(infile, line)) {
        vector<string> lines = parseCSVLine(line);
        temp.insert(pair<int, string>(atoi(lines[0].c_str()), lines[1]));
    }
    return temp;
}

// Takes single CSV line and seperates values in vector
// Also sanitizes quotation marks from output vector
vector<string> parseCSVLine(const string &line) {
    char current;
    bool isQuote = false;
    string builtStr = "";
    vector<string> outputStrings;

    // Cycle through all characters
    for(size_t i = 0; i < line.size(); ++i) {
        current = line[i];

        // Pushes string into vector when comma found
        if(!isQuote) {
            if(current == ',') {
                outputStrings.push_back(builtStr);
                builtStr = string();
            }
            else if(current == '"')
                isQuote = true;
            else
                builtStr += current;
        }

        // Checks for matching quotation marks and removes from output
        else {
            if(current == '"' && i+1 < line.size()) {
                if(line[i+1] == '"') {
                    builtStr += '"';
                    ++i;
                }
                else
                    isQuote = false;
            }
            else
                builtStr += current;
        }
    }

    return outputStrings;
}
