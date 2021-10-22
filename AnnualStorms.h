#ifndef ANNUALSTORMS_H_
#define ANNUALSTORMS_H_

#include <vector>
#include "RetVal.h"
#include "HashTable.h"
#include "Commands.h"

//main class for funtionality
class AnnualStorms {
public:
    AnnualStorms();
    ~AnnualStorms();

    RetVal Initialize(int year, int range);
    RetVal Start();

private:
    static const int MaxYear = 2021;
    static const int MinYear = 1950;

    int year;
    annual_storms **data;
    HashTable *hashTable;
    bool isInitialized;

    RetVal AddStormEvent(std::string line, int dataIndex, int eventIndex);
    RetVal AddFatalityEvent(std::string line, int dataIndex);
    RetVal ExecuteRangeCommand(std::string *args);
    void Destroy();
};

#endif /* ANNUALSTORMS_H_ */
