#include "ArgParser.h"

using namespace std;

namespace myComp {
void ArgParser::parse(int argc, char **argv) {
    // Convert to strings
    _stringfy(argc, argv);
    _program_name = _args.front();

    // If not enough args, throw error
    if (_args.size() < 2) {
        throw invalid_argument("Usage: " + _program_name +
                               " [opts] <filename>");
    }

    _file_name = _args.back();

    // Check if file exists
    if (!filesystem::exists(_file_name)) {
        throw invalid_argument("File " + _file_name + " does not exist");
    }

    // Delete the program name and the file name
    _args.erase(_args.begin());
    _args.pop_back();

    // Start parsing the arguments
    for (auto it = _args.begin(); it != _args.end(); it++) {
        if (*it == "-D") {
            _debug = true;
        } else if (*it == "-const-propagation") {
            _const_propagation = true;
        } else {
            cerr << "Unknown option: " << *it << endl;
        }
    }
}

void ArgParser::_stringfy(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        _args.push_back(argv[i]);
    }
}
} // namespace myComp