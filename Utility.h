#ifndef UTILITY_H_
#define UTILITY_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include "defns.h"
#include "RetVal.h"


class Utility {
public:
    // Typedef definitions.
    typedef bool (*CompFunc)(const std::string&, const std::string&,
            const storm_event&);
    typedef char *(*GetFunc)(storm_event&);

    //converts entered values into integers
    static RetVal ReadIntField(std::stringstream &fieldStream, int *val) {
        RetVal retVal = RetVal::NoError;
        std::string field;
        float temp = 0.0;

        // Read the next value using ',' char as delimeter
        if (std::getline(fieldStream, field, ',')) {
            // Try to convert the value to integer.
            // Catch exceptions if any occur.
            try {
                if (field.empty()) {
                    *val = 0;
                } else {
                    temp = std::stof(field);
                    *val = static_cast<int>(temp);
                    if (field.find('k') != std::string::npos
                            || field.find('K') != std::string::npos) {
                        *val *= 1000;
                    }
                }
            } catch (std::exception &e) {
                retVal = RetVal::InvalidDataRow;
            }
        } else {
            retVal = RetVal::InvalidDataRow;
        }

        return retVal;
    }


     //returns the entered value from stringstream
    static RetVal ReadStringField(std::stringstream &fieldStream, char *val,
            size_t size) {
        RetVal retVal = RetVal::NoError;
        std::string field;

        if (std::getline(fieldStream, field, ',')) {
            if (field.length() > size) {
                retVal = RetVal::InvalidDataRow;
            } else {
                memset(val, 0, size);
                strcpy(val, field.c_str());
            }
        } else {
            retVal = RetVal::InvalidDataRow;
        }

        return retVal;
    }

    //Tries to convert an inputed value to an int
    static RetVal GetInt(std::istream &input, int *val) {
        RetVal retVal = RetVal::NoError;
        std::string str;

        try {
            std::getline(input, str);
            *val = std::stoi(str);
        } catch (std::exception &e) {
            retVal = RetVal::InvalidInteger;
        }

        return retVal;
    }

    //Compare two strings up until the length of the shortest
    static int CompareString(const char *first, const char *second) {
        int retVal = 0;
        size_t firstLen = strlen(first);
        size_t secondLen = strlen(second);
        size_t compLen = firstLen > secondLen ? secondLen : firstLen;

        for (size_t i = 0; i < compLen; i++) {
            if (first[i] > second[i]) {
                retVal = 1;
                break;
            }
            else if (first[i] < second[i]) {
                retVal = -1;
                break;
            }
        }

        return retVal;
    }

    //Creates and returns a zero'd event, unsure if needed?
    static storm_event& GetNullStormEvent() {
        static storm_event *nullEventPtr;
        if (nullEventPtr == nullptr) {
            static storm_event nullEvent;
            nullEventPtr = &nullEvent;
            memset(nullEventPtr, 0, sizeof(storm_event));
        }
        return *nullEventPtr;
    }

    //Checks for null values
    static bool IsNullEvent(const storm_event &event) {
        // Only the id of null event is zero.
        if (event.event_id == 0) {
            return true;
        } else {
            return false;
        }
    }

    //return the state of a given event
    static char *GetState(storm_event &event) {
        return event.state;
    }

    //Returns the month of a given event
    static char *GetMonthName(storm_event &event) {
        return event.month_name;
    }

    //Checks to ensure the state range in valid
    static bool IsStateInRange(const std::string &low, const std::string &high,
            const storm_event &event) {
        if (CompareString(low.c_str(), event.state) <= 0
                && CompareString(high.c_str(), event.state)
                        >= 0) {
            return true;
        } else {
            return false;
        }
    }

    //checks to ensure the month range is valid
    static bool IsMonthInRange(const std::string &low, const std::string &high,
            const storm_event &event) {
        std::string month(event.month_name);
        std::transform(month.begin(), month.end(), month.begin(), ::toupper);
        int lowVal = GetMonthValue(low);
        int highVal = GetMonthValue(high);
        int monthVal = GetMonthValue(month);

        if (lowVal <= monthVal && highVal >= monthVal) {
            return true;
        }
        else {
            return false;
        }
    }

    //CONVERTS MONTHS INTO CORRESPONDING NUMERICAL VALUE
    static int GetMonthValue(const std::string &month) {
        if (month.compare("JANUARY") == 0) {
            return 1;
        }
        else if (month.compare("FEBRUARY") == 0) {
            return 2;
        }
        else if (month.compare("MARCH") == 0) {
            return 3;
        }
        else if (month.compare("APRIL") == 0) {
            return 4;
        }
        else if (month.compare("MAY") == 0) {
            return 5;
        }
        else if (month.compare("JUNE") == 0) {
            return 6;
        }
        else if (month.compare("JULY") == 0) {
            return 7;
        }
        else if (month.compare("AUGUST") == 0) {
            return 8;
        }
        else if (month.compare("SEPTEMBER") == 0) {
            return 9;
        }
        else if (month.compare("OCTOBER") == 0) {
            return 10;
        }
        else if (month.compare("NOVEMBER") == 0) {
            return 11;
        }
        else if (month.compare("DECEMBER") == 0) {
            return 12;
        }
        else {
            return 0;
        }
    }
};

#endif /* UTILITY_H_ */
