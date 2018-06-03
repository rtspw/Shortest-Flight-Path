#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>

using namespace std;

struct edge {
    int destId;
    int distance;
    string airportCode;
    string carrierId;
    string carrierName;
};

struct node {
    int id;
    double latitude;
    double longitude;
    string name;
    string code;
    string city;
};

/// Prototypes - - - - - - - - -
///
vector<string> parseCSVLine(const string &line);
void writeCSVToXML(const string &airportFile, const string &airlineFile, const string &routeFile, const string &outputFile);
unordered_map<int, string> makeIdToNameMap(const string &airlineFile);
unordered_map<int, vector<edge>> makeRouteMap(const string &routeFile, const unordered_map<int, string> &nameMap);
map<int, node> makeAirportMap(const string &airportFile);

double toRadian(const double &degree);
double getDistance(double latitude1, double longitude1, double latitude2, double longitude2);

/// Funtions - - - - - - - - - -
///
int main() {
    writeCSVToXML("airports.dat", "airlines.dat", "routes.dat", "flights.xml");
    return 0;
}

void writeCSVToXML(const string &airportFile, const string &airlineFile, const string &routeFile, const string &outputFile) {

    unordered_map<int, string> airlineNames;
    unordered_map<int, vector<edge>> routeMap;
    map<int, node> airportMap;

    ofstream fout;

    airlineNames = makeIdToNameMap(airlineFile);
    airportMap = makeAirportMap(airportFile);
    routeMap = makeRouteMap(routeFile, airlineNames);

    fout.open(outputFile.c_str());
    fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    for(auto elem : airportMap) {
        fout << "<vertex>" << endl;
        fout << "\t<id>" << elem.second.id << "</id>" << endl;
        fout << "\t<name>" << elem.second.name << "</name>" << endl;
        fout << "\t<city>" << elem.second.city << "</city>" << endl;
        fout << "\t<latitude>" << elem.second.latitude << "</latitude>" << endl;
        fout << "\t<longitude>" << elem.second.longitude << "</longitude>" << endl;
        fout << "\t<edges>" << endl;
        if (routeMap.count(elem.first) != 0) {
            for(size_t i = 0 ; i < routeMap.at(elem.first).size(); ++i) {
                fout << "\t\t<edge>" << endl;
                fout << "\t\t\t<airport>" << routeMap[elem.first][i].destId << "</airport>" << endl;
                fout << "\t\t\t<airportCode>" << routeMap[elem.first][i].airportCode << "</airportCode>" << endl;
                fout << "\t\t\t<carrierID>" << routeMap[elem.first][i].carrierId << "</carrierID>" << endl;
                fout << "\t\t\t<carrierName>" << routeMap[elem.first][i].carrierName << "</carrierName>" << endl;
                fout << "\t\t</edge>" << endl;
            }
        }
        fout << "\t</edges>" << endl;
        fout << "</vertex>" << endl;
    }
}

map<int, node> makeAirportMap(const string &airportFile) {
    ifstream infile(airportFile.c_str());
    string line;
    map<int, node> temp;

    // Seperates each line into vectors, and puts needed values in map
    while(getline(infile, line)) {
        vector<string> lines = parseCSVLine(line);
        node airport;
        airport.id = atoi(lines[0].c_str());
        airport.latitude = atof(lines[6].c_str());
        airport.longitude = atof(lines[7].c_str());
        airport.code = lines[4];
        airport.name = lines[1];
        airport.city = lines[2];
        temp.insert(pair<int, node>(airport.id, airport));
    }

    return temp;
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

// Generates hash table with Airport ID as the key and vector of all connected flights (edges) as the value
unordered_map<int, vector<edge>> makeRouteMap(const string &routeFile, const unordered_map<int, string> &nameMap) {
    ifstream infile(routeFile.c_str());
    string line;
    unordered_map<int, vector<edge>> temp;

    // Seperates each line into vectors, and puts needed values in map
    while(getline(infile, line)) {
        vector<string> lines = parseCSVLine(line);

        // Adds new hash entry if previous does not exist
        if(temp.count(3) == 0) {
            temp.insert(pair<int, vector<edge>>(atoi(lines[3].c_str()), vector<edge>()));
        }

        edge route;

        // Checks if airport exists (if id != /n)
        if(!nameMap.count(atoi(lines[1].c_str())) == 0) {
            route.destId = atoi(lines[5].c_str());
            route.airportCode = lines[4];
            route.carrierId = lines[1];
            route.carrierName = nameMap.at(atoi(lines[1].c_str()));
            temp[atoi(lines[3].c_str())].push_back(route);
        }
    }
    return temp;
}

// Takes single CSV line and seperates values in vector
// Also sanitizes quotation marks from output vector
// Input example: " item1, "item2", ... "
// Output example: vector([0]: item1, [1]: item2, [2]: ...)
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

// Helper function : Converts degress to radians
double toRadian(const double &degree) {
    return degree * M_PI / 180.0;
}

// Helper function : Calculates distance (miles) between two points on Earth
double getDistance(double latitude1, double longitude1, double latitude2, double longitude2) {
    double lat1 = toRadian(latitude1);
    double lon1 = toRadian(longitude1);
    double lat2 = toRadian(latitude2);
    double lon2 = toRadian(longitude2);
    double deltaLat = abs(lat1 - lat2);
    double deltaLon = abs(lon1 - lon2);
    double a = pow(sin(deltaLat/2), 2) + (cos(lat1)*cos(lat2)*pow(sin(deltaLon/2), 2));
    double c = 2 * asin(sqrt(a));
    return 3959 * c;
}
