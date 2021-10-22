#include <iostream>
#include "AnnualStorms.h"
#include "Utility.h"

//Main function for project 2
int main(int argc, char *argv[]) {
    RetVal retVal = RetVal::NoError;
    AnnualStorms storms;
    int startYear = 0;
    int range = 0;

    // Check number of arguments.
    if (argc != 3) {
        std::cerr << "Invalid number of arguments!\n";
        return EXIT_FAILURE;
    }

    // Validate and set arguments.
    try {
        startYear = std::stoi(argv[1]);
        range = std::stoi(argv[2]);
    }
    catch (std::exception &e) {
        std::cerr << "Invalid arguments!\n";
        return EXIT_FAILURE;
    }

    // Initialize AnnualStorms instance.
    retVal = storms.Initialize(startYear, range);
    // If fails, the return EXIT_FAILURE.
    if(retVal != RetVal::NoError) {
        return EXIT_FAILURE;
    }

    // Start the main application.
    retVal = storms.Start();
    // If fails, the return EXIT_FAILURE.
    if(retVal != RetVal::NoError) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
