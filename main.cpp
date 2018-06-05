#include <iostream>
#include "controller.h"

using namespace std;

/// Prototypes - - - - - - - - -
///
string getInput(const string &question, const bool &allCaps);
void getStartEndAirports(string &startAirport, string &endAirport);
void askToOutputXML(Controller &c);
void printItinerary(const vector<string> &itinerary);
bool isValidIATAFormat(const string &code);
void capitalizeText(string &text);

/// Functions - - - - - - - - - -
///
int main() {
    Controller mainC("airports.dat", "airlines.dat", "routes.dat");
    string startAirportCode, endAirportCode;
    try {
        getStartEndAirports(startAirportCode, endAirportCode);
        printItinerary(mainC.getShortestPath(startAirportCode, endAirportCode));
        askToOutputXML(mainC);
    }
    catch (CONTROLLER_ERRORS e) {
        if(e == START_NOT_FOUND)
            cout << "ERROR: Starting airport not found.";
        else if(e == END_NOT_FOUND)
            cout << "ERROR: Ending airport not found.";
        else if(e == START_END_SAME)
            cout << "ERROR: Start and ending airport is the same.";
        else if(e == NO_ROUTE_FOUND)
            cout << "ERROR: No possible route found. Airports may be non-commercial.";
        else if(e == INVALID_FILENAME)
            cout << "ERROR: Invalid filename. Must end with '.xml'";
        cout << endl;
    }
    catch (...) {
        cout << "ERROR: Unknown Error Occured!" << endl;
    }
    return 0;
}

// Asks user for start and end IATA code, while checking for errors in input
void getStartEndAirports(string &startAirport, string &endAirport) {

    bool valid = false;

    while(!valid) {

        startAirport = getInput("Insert IATA code of starting airport: ", true);
        while(!isValidIATAFormat(startAirport)) {
            startAirport = getInput("ERROR: Improper format. Try again: ", true);
        }

        endAirport = getInput("Insert IATA code of ending airport: ", true);
        while(!isValidIATAFormat(endAirport)) {
            endAirport = getInput("ERROR: Improper format. Try again: ", true);
        }

        valid = true;
        if(startAirport == endAirport) {
            cout << "ERROR: Can not be the same airport!" << endl;
            valid = false;
        }
    }
}

// Asks user if they want to output all airport info to an XML
void askToOutputXML(Controller &c) {
    string input = getInput("Print all airport information to XML? (Y/N): ", false);
    if(!input.empty() && toupper(input[0]) == 'Y') {
        string filename = getInput("Print filename for XML (Must end in .xml): ", false);
        c.writeCSVToXML(filename);
    }
    cout << "...Done" << endl << endl;
}

// Prints a vector of strings containing route instructions
void printItinerary(const vector<string> &itinerary) {
    cout << endl << " - - - ITINERARY - - - " << endl << endl;
    for(size_t i = 0 ; i < itinerary.size(); ++i) {
        cout << itinerary[i] << endl << endl;
    }
    cout << " - - - - - - - - - - - " << endl << endl;
}

// Asks for input using "question", and can return it in all caps
string getInput(const string &question, const bool &allCaps) {
    string line;
    cout << question;
    getline(cin, line);
    if(allCaps)
        capitalizeText(line);
    return line;
}

// Returns true if string is 3 alphabetical characters (IATA style)
bool isValidIATAFormat(const string &code) {
    if(code.size() != 3)
        return false;
    bool isAllAlphabet = true;
    for(size_t i = 0; i < code.size(); ++i)
        if(!isalpha(code[i]))
            isAllAlphabet = false;
    return isAllAlphabet;
}


// Helper function: converts string to ALL CAPS
void capitalizeText(string &text) {
    for(size_t i = 0; i < text.size(); ++i)
        text[i] = toupper(text[i]);
}

