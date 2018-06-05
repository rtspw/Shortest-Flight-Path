#include "controller.h"

using namespace std;

/// CONSTRUCTOR & INITIALIZATION FUNCTIONS
///

Controller::Controller(const string &airportFile, const string &airlineFile, const string &routeFile) {
    this->airportFile = airportFile;
    this->airlineFile = airlineFile;
    this->routeFile = routeFile;
    constructMaps();
}

Controller::~Controller() {
    deleteAll();
}

Controller::Controller(const Controller &other) {
    copy(other);
}

Controller &Controller::operator=(const Controller &other) {
    if(this != &other) {
        deleteAll();
        copy(other);
    }
    return *this;
}

// Processes CSV data files and generates four maps used for lookup for the information
void Controller::constructMaps() {
    makeIdToNameMap(); // Map for airline id to airline name
    makeAirportMap();  // Maps for airport id to airport node, and airport IATA to airport id
    makeRouteMap();    // Maps for airport id to vector of all connecting edges
}

/// PUBLIC FUNCTIONS
///

// Traverses the graph (adjacency list provided by "routeMap") and uses the Dijkstra's algorithm
// to find the shortest path using avaliable flights between airports.
// Returns a deque containing airport id's in the path, which should be traversed
// from front to back to be in order
std::vector<std::string> Controller::getShortestPath(const std::string &start, const std::string &end) {

    if(!nameToIdMap.count(start))
        throw START_NOT_FOUND;

    if(!nameToIdMap.count(end))
        throw END_NOT_FOUND;

    if(start == end)
        throw START_END_SAME;

    int startId = nameToIdMap[start];
    int endId = nameToIdMap[end];

    priority_queue<pair<double, edge>> pq;
    unordered_set<int> processed;
    unordered_map<int, int> parentOfId;

    // Gets vector with all connecting routes with an airport
    vector<edge> connectingEdges = routeMap[startId];
    processed.insert(startId);

    // Starts initial case of shortest path algorithm
    for(size_t i = 0; i < connectingEdges.size(); ++i) {
        pq.push(pair<double, edge>(-1 * connectingEdges[i].distance, connectingEdges[i]));
    }

    int nextId = -1;
    double nextDistance;

    // Loops through until shortest path found or no possible route
    while(!pq.empty() && nextId != endId) {

        nextDistance = pq.top().first;
        nextId = pq.top().second.destId;
        edge temp = pq.top().second;
        pq.pop();

        if(processed.find(nextId) == processed.end()) {
            parentOfId.insert(pair<int, int>(temp.destId, temp.sourceId));
            connectingEdges = routeMap[nextId];
            for(size_t i = 0; i < connectingEdges.size(); ++i) {
                pq.push(pair<double, edge>(-1*connectingEdges[i].distance + nextDistance, connectingEdges[i]));
            }
            processed.insert(nextId);
        }
    }

    // Throws error if no possible routes between airports
    // due to closed airports or private/non-commercial airports
    if(pq.empty() && nextId != endId)
        throw NO_ROUTE_FOUND;

    // Puts resulting path by traversing parents in deque to ease reversing results
    deque<int> path;
    int parent = endId;
    while(parent != startId) {
        path.push_front(parent);
        parent = parentOfId[parent];
    }
    path.push_front(parent);

    // Creates the string itinerary to be returned
    vector<string> itinerary;
    makeItinerary(path, itinerary);
    return itinerary;
}

// Takes an output file name and generates an XML file
// containing all verticies (airports) and edges (routes)
void Controller::writeCSVToXML(const string &outputFile) {

    if(outputFile.length() < 5 || outputFile.substr(outputFile.length() - 4) != ".xml")
        throw INVALID_FILENAME;

    ofstream fout;
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
                fout << "\t\t\t<airportID>" << routeMap[elem.first][i].destId << "</airportID>" << endl;
                fout << "\t\t\t<airportCode>" << routeMap[elem.first][i].airportCode << "</airportCode>" << endl;
                fout << "\t\t\t<carrierID>" << routeMap[elem.first][i].carrierId << "</carrierID>" << endl;
                fout << "\t\t\t<carrierName>" << routeMap[elem.first][i].carrierName << "</carrierName>" << endl;
                fout << "\t\t\t<distance>" << routeMap[elem.first][i].distance << "</distance>" << endl;
                fout << "\t\t</edge>" << endl;
            }
        }
        fout << "\t</edges>" << endl;
        fout << "</vertex>" << endl;
    }
}

// Finds all edges (airlines) between two certain nodes (airports)
// Warning: Does not check for existance in map so must already be validated
std::vector<edge> Controller::findEdgesBetweenNodes(const int &aId, const int &bId) {
    std::vector<edge> allConnectingEdges = routeMap[aId];
    std::vector<edge> correspondingEdges;
    for(size_t i = 0; i < allConnectingEdges.size(); ++i) {
        if(allConnectingEdges[i].destId == bId) {
            correspondingEdges.push_back(allConnectingEdges[i]);
        }
    }
    return correspondingEdges;
}

// Generates a hash table where key: (airline id) and value: (airline name)
// Can be used to convert an airline id to its name (string)
void Controller::makeIdToNameMap() {
    ifstream infile(airlineFile.c_str());
    string line;

    // Seperates each line into vectors, and puts needed values in map
    while(getline(infile, line)) {
        vector<string> lines = parseCSVLine(line);
        airlineNames.insert(pair<int, string>(atoi(lines[0].c_str()), lines[1]));
    }
    infile.close();
}

// Generates hash table with where key: (airport id) and value: (connecting edges)
// Can be used to get a vector of all routes (edges) avaliable at an airport
void Controller::makeRouteMap() {
    ifstream infile(routeFile.c_str());
    string line;

    // Seperates each line into vectors, and puts needed values in map
    while(getline(infile, line)) {
        vector<string> lines = parseCSVLine(line);

        int sourceId = atoi(lines[SOURCE_AIRPORT_ID].c_str());
        int destinationId = atoi(lines[DESTINATION_AIRPORT_ID].c_str());

        // Adds entry to routeMap if both source and destination exists
        if(airportMap.count(sourceId) && airportMap.count(destinationId)) {
            edge route;

            // Adds new hash entry if previous does not exist
            if(!routeMap.count(sourceId)) {
                routeMap.insert(pair<int, vector<edge>>(sourceId, vector<edge>()));
            }

            route.destId = destinationId;
            route.sourceId = sourceId;
            route.airportCode = lines[DESTINATION_AIRPORT_IATA];
            route.carrierId = atoi(lines[CARRIER_ID].c_str());

            // Checks if airport exists (if id != /n)
            if(airlineNames.count(route.carrierId))
                route.carrierName = airlineNames.at(route.carrierId);
            else
                route.carrierName = "Unknown Carrier";

            // Calculates distance using lat and longitudes of source and destination
            double sourceLat = airportMap.at(sourceId).latitude;
            double sourceLon = airportMap.at(sourceId).longitude;
            double destLat = airportMap.at(destinationId).latitude;
            double destLon = airportMap.at(destinationId).longitude;
            route.distance = getDistance(sourceLat, sourceLon, destLat, destLon);

            // Pushes new route into route vector
            routeMap[sourceId].push_back(route);
        }
    }
    infile.close();
}

// Creates a map where key: (id) and value: (node)
// As such, it can be used to retrieve info about an airport using the id as a lookup
// Also makes second hashtable key: (IATA) and value: (id) to translate user input
void Controller::makeAirportMap() {
    ifstream infile(airportFile.c_str());
    string line;

    // Seperates each line into vectors, and puts needed values in map
    while(getline(infile, line)) {
        vector<string> lines = parseCSVLine(line);
        node airport;
        airport.id = atoi(lines[AIRPORT_ID].c_str());
        airport.latitude = atof(lines[AIRPORT_LATITUDE].c_str());
        airport.longitude = atof(lines[AIRPORT_LONGITUDE].c_str());
        airport.code = lines[AIRPORT_IATA];
        airport.name = lines[AIRPORT_NAME];
        airport.city = lines[AIRPORT_CITY];
        airportMap.insert(pair<int, node>(airport.id, airport));
        nameToIdMap.insert(pair<string, int>(airport.code, airport.id));
    }
    infile.close();
}

// Creates a vector of string, with the directions/itinerary in order
// For example: [0] : "Start from Starting Airport"
// [1] : "Fly to Other Airport for x miles using\n
//           - Airline 1
//           - Airline 2...
// [2] : Arrive at Ending Airport
void Controller::makeItinerary(std::deque<int> &path, std::vector<std::string> &itinerary) {
    string buildStr;
    int previousCode = path.front();
    buildStr = "Start from " + airportMap[previousCode].code + " (" + airportMap[previousCode].name + ")";
    itinerary.push_back(buildStr);
    path.pop_front();

    // Creates text for all flights in between the start and end
    // Input varies on whether there are multiple airlines or just one
    while(path.size() != 0) {
        vector<edge> connectingRoutes = findEdgesBetweenNodes(previousCode, path.front());
        buildStr = "Fly to " + airportMap[path.front()].code + " (" + airportMap[path.front()].name
                 + ") over " + to_string((int)connectingRoutes[0].distance) + " miles using";

        // Either lists airlines or uses one
        if(connectingRoutes.size() == 1) {
            buildStr += " " + connectingRoutes[0].carrierName;
        }
        else {
            buildStr += " one of the following:";
            for(size_t i = 0; i < connectingRoutes.size(); ++i) {
                buildStr += "\n  - " + connectingRoutes[i].carrierName;
            }
        }
        itinerary.push_back(buildStr);
        previousCode = path.front();

        // Final arrival message when on the last entry
        if(path.size() == 1) {
            buildStr = "Arrive at " + airportMap[path.front()].code + " (" + airportMap[path.front()].name + ")";
            itinerary.push_back(buildStr);
        }
        path.pop_front();
    }
}

/// HELPER FUNCTIONS
///

// Takes single CSV line and seperates values in vector
// Also sanitizes quotation marks from output vector
// Input example: " item1, "item2", ... "
// Output example: vector([0]: item1, [1]: item2, [2]: ...)
vector<string> Controller::parseCSVLine(const string &line) {
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
double Controller::toRadian(const double &degree) {
    return degree * M_PI / 180.0;
}

// Helper function : Calculates distance (miles) between two points on Earth
double Controller::getDistance(double latitude1, double longitude1, double latitude2, double longitude2) {
    double lat1 = toRadian(latitude1);
    double lon1 = toRadian(longitude1);
    double lat2 = toRadian(latitude2);
    double lon2 = toRadian(longitude2);
    double deltaLat = abs(lat1 - lat2);
    double deltaLon = abs(lon1 - lon2);
    double a = pow(sin(deltaLat/2), 2) + (cos(lat1)*cos(lat2)*pow(sin(deltaLon/2), 2));
    double c = 2 * asin(sqrt(a));
    const int EARTH_RADIUS = 3959;
    return EARTH_RADIUS * c;
}

void Controller::copy(const Controller &other) {
    airportFile = other.airportFile;
    airlineFile = other.airlineFile;
    routeFile = other.routeFile;
    airlineNames = other.airlineNames;
    routeMap = other.routeMap;
    nameToIdMap = other.nameToIdMap;
    airportMap = other.airportMap;
}

void Controller::deleteAll() {
    airportFile = string();
    airlineFile = string();
    routeFile = string();
    airlineNames.clear();
    routeMap.clear();
    nameToIdMap.clear();
    airportMap.clear();
}
