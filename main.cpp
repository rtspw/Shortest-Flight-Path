#include <iostream>
#include "controller.h"

using namespace std;

/// Prototypes - - - - - - - - -
///

/// Functions - - - - - - - - - -
///
int main() {
    Controller mainC("airports.dat", "airlines.dat", "routes.dat");
    mainC.getShortestPath("TPE", "HAD");
    mainC.writeCSVToXML("f.txtxml");
    return 0;
}





