#ifndef RETVAL_H_
#define RETVAL_H_

//provides error codes
class RetVal {
public:
    enum Enum {
        NoError = 0,
        InvalidYear,
        InvalidDataRow,
        InvalidFatalityData,
        InvalidInteger,
        InvalidFieldName,
        InvalidMonth,
        FileIOError,
        FileReadFailed,
        CommandReadFailed,
        SyntaxError
    };

    RetVal(Enum value = NoError) :
            value(value) {
    }

    operator Enum() {
        return value;
    }

    const char* toString() const {
        switch (value) {
        case NoError:
            return "No error";
        case InvalidYear:
            return "Invalid year";
        case InvalidDataRow:
            return "Invalid data row";
        case InvalidFatalityData:
            return "Invalid fatality data";
        case InvalidInteger:
            return "Invalid integer";
        case InvalidFieldName:
            return "Invalid field name";
        case InvalidMonth:
            return "Invalid month";
        case FileIOError:
            return "File I/O error";
        case FileReadFailed:
            return "File read failed.";
        case CommandReadFailed:
            return "Command read failed.";
        case SyntaxError:
            return "Syntax error.";
        default:
            return "Unknown error";
        }
    }

private:
    Enum value;
};

#endif /* RETVAL_H_ */
