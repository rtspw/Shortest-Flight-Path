#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <queue>
#include <cmath>
#include <fstream>
#include <sstream>
#include <deque>

#ifndef CONTROLLER_H
#define CONTROLLER_H

enum airportCSVMeanings {
    AIRPORT_ID,
    AIRPORT_NAME,
    AIRPORT_CITY,
    AIRPORT_COUNTRY,
    AIRPORT_IATA,
    AIRPORT_ICAO,
    AIRPORT_LATITUDE,
    AIRPORT_LONGITUDE
};

enum routeCSVMeanings {
    CARRIER_CODE,
    CARRIER_ID,
    SOURCE_AIRPORT_IATA,
    SOURCE_AIRPORT_ID,
    DESTINATION_AIRPORT_IATA,
    DESTINATION_AIRPORT_ID
};

enum CONTROLLER_ERRORS {
    START_NOT_FOUND,
    END_NOT_FOUND,
    START_END_SAME,
    NO_ROUTE_FOUND,
    INVALID_FILENAME
};

struct edge {
    int destId;
    int sourceId;
    int carrierId;
    double distance;
    std::string airportCode;
    std::string carrierName;

    friend
    bool operator<(const edge &a, const edge &b) {
        return a.distance < b.distance;
    }
    friend
    bool operator>(const edge &a, const edge &b) {
        return a.distance > b.distance;
    }
};

struct node {
    int id;
    double latitude;
    double longitude;
    std::string name;
    std::string code;
    std::string city;
};


class Controller {

public:

    Controller(const std::string &airportFile, const std::string &airlineFile, const std::string &routeFile);
    ~Controller();
    Controller(const Controller &other);
    Controller &operator=(const Controller &other);

    void writeCSVToXML(const std::string &outputFile);
    std::vector<std::string> getShortestPath(const std::string &start, const std::string &end);
    std::vector<edge> findEdgesBetweenNodes(const int &aId, const int &bId);


private:

    std::string airportFile;
    std::string airlineFile;
    std::string routeFile;

    std::unordered_map<int, std::string> airlineNames;
    std::unordered_map<int, std::vector<edge>> routeMap;
    std::unordered_map<std::string, int> nameToIdMap;
    std::map<int, node> airportMap;


    void constructMaps();
    void makeIdToNameMap();
    void makeRouteMap();
    void makeAirportMap();
    void makeItinerary(std::deque<int> &path, std::vector<std::string> &itinerary);
    double toRadian(const double &degree);
    double getDistance(double latitude1, double longitude1, double latitude2, double longitude2);

    std::vector<std::string> parseCSVLine(const std::string &line);

    void copy(const Controller &other);
    void deleteAll();
};

#endif // CONTROLLER_H
