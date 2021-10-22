#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <vector>
#include <string>
#include <sstream>
#include "RetVal.h"

//support for commands
class Commands {
public:
    enum Enum {
        None = 0, FindEvent, FindMax, FindMaxFatality, Range, CommandCount
    };

    Commands(Enum value = None) :
            value(value), arguments(nullptr) {

    }

    operator Enum() const {
        return value;
    }

    const char* GetString() const {
        switch (value) {
        case FindEvent:
            return "find event";
        case FindMax:
            return "find max";
        case FindMaxFatality:
            return "find max fatality";
        case Range:
            return "range";
        default:
            return "unknown command";
        }
    }


    RetVal SetFromString(std::string line) {
        RetVal retVal = RetVal::NoError;
        std::stringstream tokenStream(line);
        std::string token;
        Reset();

        // Read the first token
        if (tokenStream >> token) {
            // If it is a 'range' command then, it must be followed by 4 arguments.
            if (token.compare("range") == 0) {
                value = Range;
                arguments = new std::string[NumberOfRangeCommandArgs];
                for (int i = 0; i < NumberOfRangeCommandArgs; i++) {
                    if (tokenStream >> token) {
                        arguments[i] = token;
                    } else {
                        retVal = RetVal::SyntaxError;
                        break;
                    }
                }
            }
            // If it is a 'find' command it could be 'find event', 'find max' or 'find max fatality'
            else if (token.compare("find") == 0) {
                // Read the next token.
                if (tokenStream >> token) {
                    // If it is a 'find event' command then, it must be followed by 1 argument.
                    if (token.compare("event") == 0) {
                        value = FindEvent;
                        arguments = new std::string[NumberOfFindEventArgs];
                        for (int i = 0; i < NumberOfFindEventArgs; i++) {
                            if (tokenStream >> token) {
                                arguments[i] = token;
                            } else {
                                retVal = RetVal::SyntaxError;
                                break;
                            }
                        }
                    }
                    // If the second token is 'max' then, it could be 'find max' or 'find max fatality'
                    else if (token.compare("max") == 0) {
                        // Read the next token.
                        if (tokenStream >> token) {
                            // If it is a 'find max fatality' command then, it must be followed by2 args.
                            if (token.compare("fatality") == 0) {
                                value = FindMaxFatality;
                                arguments =
                                        new std::string[NumberOfFindMaxFatalityArgs];
                                for (int i = 0; i < NumberOfFindMaxFatalityArgs;
                                        i++) {
                                    if (tokenStream >> token) {
                                        arguments[i] = token;
                                    } else {
                                        retVal = RetVal::SyntaxError;
                                        break;
                                    }
                                }
                            }
                            // If the next token is not 'fatality' then it must be 'find max' command
                            // which must be followed by 3 args.
                            else {
                                value = FindMax;
                                arguments =
                                        new std::string[NumberOfFindMaxArgs];
                                // first arg was already read.
                                arguments[0] = token;
                                for (int i = 1; i < NumberOfFindMaxArgs; i++) {
                                    if (tokenStream >> token) {
                                        arguments[i] = token;
                                    } else {
                                        retVal = RetVal::SyntaxError;
                                        break;
                                    }
                                }
                            }
                        } else {
                            retVal = RetVal::SyntaxError;
                        }
                    } else {
                        retVal = RetVal::SyntaxError;
                    }
                } else {
                    retVal = RetVal::SyntaxError;
                }
            }
            // If the first token is neither 'range' or 'find' then, it is a syntax error.
            else {
                retVal = RetVal::SyntaxError;
            }
        }
        // Failed to read the first token.
        else {
            retVal = RetVal::CommandReadFailed;
        }

        if (retVal != RetVal::NoError) {
            Reset();
        }

        return retVal;
    }

    std::string *GetArguments() {
        return arguments;
    }

private:
    static const int NumberOfRangeCommandArgs = 4;
    static const int NumberOfFindEventArgs = 1;
    static const int NumberOfFindMaxArgs = 3;
    static const int NumberOfFindMaxFatalityArgs = 2;
    Enum value;
    std::string *arguments;

    void Reset() {
        value = None;
        delete[] arguments;
        arguments = nullptr;
    }
};

#endif /* COMMANDS_H_ */
