#include "AnnualStorms.h"
#include "Utility.h"
#include "BinarySearchTree.h"


#define NULL_EVENT Utility::GetNullStormEvent()
#define IS_NULL(e) Utility::IsNullEvent(e)

AnnualStorms::AnnualStorms() :
        year(0), data(nullptr), hashTable(nullptr), isInitialized(false) {
}


AnnualStorms::~AnnualStorms() {
    Destroy();
}

//Takes a starting year and a range
RetVal AnnualStorms::Initialize(int year, int range) {
    RetVal retVal = RetVal::NoError;
    std::ifstream *detailsFiles[range];
    std::ifstream *fatalityFiles[range];
    int stormRowCounts[range];
    int fatalityRowCounts[range];
    std::string detailCsv;
    std::string fatalityCsv;
    std::string line;
    int totalDataCount = 0;
    int fatalityIndex = 0;
    int eventIndex = 0;

    // Check if the year is within the valid range.
    if (year < MinYear || year + range - 1 > MaxYear) {
        return RetVal::InvalidYear;
    } else {
        // Set year and paths of teh input files.
        this->year = year;
        data = new annual_storms*[range + 1];
        data[range] = nullptr;
        for (int i = 0; i < range; i++) {
            data[i] = new annual_storms { year + i, nullptr };
            detailCsv = std::string(
                    "details-" + std::to_string(year + i) + ".csv");
            fatalityCsv = std::string(
                    "fatalities-" + std::to_string(year + i) + ".csv");
            detailsFiles[i] = new std::ifstream(detailCsv, std::ifstream::in);
            fatalityFiles[i] = new std::ifstream(fatalityCsv,
                    std::ifstream::in);
        }
    }

    // Count the number of lines in input files.
    for (int i = 0; data[i] != nullptr && retVal == RetVal::NoError; i++) {
        if (!detailsFiles[i]->is_open() || !fatalityFiles[i]->is_open()) {
            retVal = RetVal::FileIOError;
        } else {
            stormRowCounts[i] = std::count(
                    std::istreambuf_iterator<char>(*detailsFiles[i]),
                    std::istreambuf_iterator<char>(), '\n');
            fatalityRowCounts[i] = std::count(
                    std::istreambuf_iterator<char>(*fatalityFiles[i]),
                    std::istreambuf_iterator<char>(), '\n');
            data[i]->events = new storm_event[stormRowCounts[i] + 1];
            data[i]->events[stormRowCounts[i]] = NULL_EVENT;
            totalDataCount += stormRowCounts[i];
        }
    }

    // If no error occurred up to this point then, create the hash table.
    if (retVal == RetVal::NoError) {
        hashTable = new HashTable(totalDataCount);
    }

    // Read input files.
    for (int i = 0; data[i] != nullptr && retVal == RetVal::NoError; i++) {
        // Reset eventIndex and fatalityIndex for the current year's data.
        eventIndex = 0;
        fatalityIndex = 0;

        // Reset input streams and seek pointers which
        // were used for counting the number of lines.
        detailsFiles[i]->clear();
        fatalityFiles[i]->clear();
        detailsFiles[i]->seekg(0, std::ios::beg);
        fatalityFiles[i]->seekg(0, std::ios::beg);

        // Read and ignore the first line which includes not data but field names.
        if (!std::getline(*detailsFiles[i], line)) {
            retVal = RetVal::FileReadFailed;
        }

        // Read storm data line by line.
        while (retVal == RetVal::NoError && eventIndex < stormRowCounts[i]) {
            if (!std::getline(*detailsFiles[i], line)) {
                retVal = RetVal::FileReadFailed;
                break;
            }

            // Add a storm event.
            retVal = AddStormEvent(line, i, eventIndex);
            if (retVal != RetVal::NoError) {
                std::cerr << retVal.toString() << ". Row: " << eventIndex
                        << "\n";
            }
            eventIndex++;
        }

        // If successfully read the storm event data next,
        // read the current year's fatality data.
        if (retVal == RetVal::NoError) {
            if (!std::getline(*fatalityFiles[i], line)) {
                retVal = RetVal::FileReadFailed;
            }

            while (retVal == RetVal::NoError
                    && fatalityIndex < fatalityRowCounts[i]) {
                if (!std::getline(*fatalityFiles[i], line)) {
                    retVal = RetVal::FileReadFailed;
                    break;
                }

                retVal = AddFatalityEvent(line, i);
                if (retVal != RetVal::NoError) {
                    std::cerr << retVal.toString() << ". Fatality: "
                            << fatalityIndex << "\n";
                }
                fatalityIndex++;
            }
        }

        // Close the files.
        detailsFiles[i]->close();
        fatalityFiles[i]->close();
    }

    // If an error occurred free the dynamically allocated memory before returning.
    if (retVal != RetVal::NoError) {
        Destroy();
    } else {
        isInitialized = true;
    }

    // Delete dynamically allocated input file streams
    for (int i = 0; i < range; i++) {
        delete detailsFiles[i];
    }

    for (int i = 0; i < range; i++) {
        delete fatalityFiles[i];
    }

    return retVal;
}

//Creates a storm value
RetVal AnnualStorms::AddStormEvent(std::string line, int dataIndex,
        int eventIndex) {
    RetVal retVal = RetVal::NoError;
    std::stringstream fieldStream(line);
    std::string field;
    storm_event *stormEvent = &(data[dataIndex]->events[eventIndex]);

    retVal = Utility::ReadIntField(fieldStream, &(stormEvent->event_id));

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadStringField(fieldStream, stormEvent->state,
                sizeof(stormEvent->state));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadIntField(fieldStream, &(stormEvent->year));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadStringField(fieldStream, stormEvent->month_name,
                sizeof(stormEvent->month_name));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadStringField(fieldStream, stormEvent->event_type,
                sizeof(stormEvent->event_type));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadStringField(fieldStream, &(stormEvent->cz_type),
                sizeof(stormEvent->cz_type));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadStringField(fieldStream, stormEvent->cz_name,
                sizeof(stormEvent->cz_name));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadIntField(fieldStream,
                &(stormEvent->injuries_direct));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadIntField(fieldStream,
                &(stormEvent->injuries_indirect));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadIntField(fieldStream,
                &(stormEvent->deaths_direct));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadIntField(fieldStream,
                &(stormEvent->deaths_indirect));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadIntField(fieldStream,
                &(stormEvent->damage_property));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadIntField(fieldStream,
                &(stormEvent->damage_crops));
    }

    // If no error occurred then, also insert the event to the hash table.
    if (retVal == RetVal::NoError) {
        stormEvent->f = nullptr;
        hashTable->Insert(*stormEvent, eventIndex);
    }

    return retVal;
}

//Adds a fatality to the hashtable
RetVal AnnualStorms::AddFatalityEvent(std::string line, int dataIndex) {
    RetVal retVal = RetVal::NoError;
    std::stringstream fieldStream(line);
    std::string field;
    fatality_event *fatalityEvent = new fatality_event();
    int eventIndex = -1;

    retVal = Utility::ReadIntField(fieldStream, &(fatalityEvent->fatality_id));

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadIntField(fieldStream, &(fatalityEvent->event_id));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadStringField(fieldStream,
                &(fatalityEvent->fatality_type),
                sizeof(fatalityEvent->fatality_type));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadStringField(fieldStream,
                fatalityEvent->fatality_date,
                sizeof(fatalityEvent->fatality_date));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadIntField(fieldStream,
                &(fatalityEvent->fatality_age));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadStringField(fieldStream,
                &(fatalityEvent->fatality_sex),
                sizeof(fatalityEvent->fatality_sex));
    }

    if (retVal == RetVal::NoError) {
        retVal = Utility::ReadStringField(fieldStream,
                fatalityEvent->fatality_location,
                sizeof(fatalityEvent->fatality_location));
    }

    // If no error occurred then, use the hash table to find which event this fatality data
    // belongs to and add it to its fatality list.
    if (retVal == RetVal::NoError) {
        fatalityEvent->next = nullptr;
        eventIndex = hashTable->GetEventIndex(fatalityEvent->event_id,
                year + dataIndex);
        if (eventIndex == -1) {
            retVal = RetVal::InvalidFatalityData;
        } else {
            fatality_event *current = data[dataIndex]->events[eventIndex].f;
            if (current == nullptr) {
                data[dataIndex]->events[eventIndex].f = fatalityEvent;
            } else {
                while (current->next != nullptr) {
                    current = current->next;
                }
                current->next = fatalityEvent;
            }
        }
    } else {
        delete fatalityEvent;
    }

    return retVal;
}

//Handles input from the user, gives errors if invalid
//REPLACE VECTORS CANNOT USE
RetVal AnnualStorms::Start() {
    RetVal retVal = RetVal::NoError;
    std::vector<Commands> commands; //Could not find alternative in time
    Commands currentCommand;
    std::string line = "";
    int commandsCount = 0;

    retVal = Utility::GetInt(std::cin, &commandsCount);
    for (int i = 0; retVal == RetVal::NoError && i < commandsCount; i++) {
        std::cout << "\nQuery:";
        std::cout.flush();
        if (!std::getline(std::cin, line) || line.empty()) {
            retVal = RetVal::SyntaxError;
            break;
        }
        std::cout << "\n";

        retVal = currentCommand.SetFromString(line);
        if (retVal != RetVal::NoError) {
            std::cerr << retVal.toString() << "\n";
            std::cerr.flush();
            continue;
        }

        std::string *args = currentCommand.GetArguments();
        switch (currentCommand) {
        case Commands::Range:
            retVal = ExecuteRangeCommand(args);
            break;
        case Commands::FindEvent:
            break;
        case Commands::FindMax:
            break;
        case Commands::FindMaxFatality:
            break;
        default:
            break;
        }
    }

    return retVal;
}

//Functionality for the "Range" command
RetVal AnnualStorms::ExecuteRangeCommand(std::string *args) {
    RetVal retVal = RetVal::NoError;
    annual_storms *dataInRange[MaxYear - MinYear] = { nullptr };
    Utility::CompFunc compFunc = nullptr;
    Utility::GetFunc getFunc = nullptr;
    BinarySearchTree *binarySearchTree = nullptr;
    std::string low = "";
    std::string high = "";

    if (args[0].compare("all") == 0) {
        for (int i = 0; data[i] != nullptr; i++) {
            dataInRange[i] = data[i];
        }
    } else {
        int year = 0;
        std::istringstream inputStream(args[0]);
        retVal = Utility::GetInt(inputStream, &year);
        if (retVal == RetVal::NoError) {
            for (int i = 0; data[i] != nullptr; i++) {
                if (data[i]->year == year) {
                    dataInRange[0] = data[i];
                    break;
                }
            }
        }

        if (dataInRange[0] == nullptr) {
            retVal = RetVal::InvalidYear;
        }
    }

    if (retVal == RetVal::NoError) {
        binarySearchTree = new BinarySearchTree(dataInRange, args[1]);
        args[2].erase(std::remove(args[2].begin(), args[2].end(), '\"'),
                args[2].end());
        args[3].erase(std::remove(args[3].begin(), args[3].end(), '\"'),
                args[3].end());
        low = args[2];
        high = args[3];

        if (args[1].compare("state") == 0) {
            compFunc = Utility::IsStateInRange;
            getFunc = Utility::GetState;
        } else if (args[1].compare("month_name") == 0) {
            compFunc = Utility::IsMonthInRange;
            getFunc = Utility::GetMonthName;
            std::transform(low.begin(), low.end(), low.begin(), ::toupper);
            std::transform(high.begin(), high.end(), high.begin(), ::toupper);
            if (Utility::GetMonthValue(low) == 0
                    || Utility::GetMonthValue(high) == 0) {
                retVal = RetVal::InvalidMonth;
            }
        } else {
            retVal = RetVal::InvalidFieldName;
        }

        if (retVal == RetVal::NoError) {
            for (int i = 0; dataInRange[i] != nullptr; i++) {
                for (int j = 0; !IS_NULL(dataInRange[i]->events[j]); j++) {
                    if (compFunc(low, high, dataInRange[i]->events[j])) {
                        binarySearchTree->Insert(
                                getFunc(dataInRange[i]->events[j]),
                                dataInRange[i]->events[j].event_id,
                                dataInRange[i]->events[j].year, j);
                    }
                }
            }
        }

        if (retVal == RetVal::NoError) {
            binarySearchTree->Print();
        }

        delete binarySearchTree;
    }

    return retVal;
}

//Frees up allocated memory
void AnnualStorms::Destroy() {
    for (int i = 0; data[i] != nullptr; i++) {
        if (data[i]->events != nullptr) {
            delete[] data[i]->events;
            data[i]->events = nullptr;
            delete data[i];
            data[i] = nullptr;
        }
    }

    if (hashTable != nullptr) {
        delete hashTable;
        hashTable = nullptr;
    }

    isInitialized = false;
}
