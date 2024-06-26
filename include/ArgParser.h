#ifndef MYCOMP_ARGPARSER_H
#define MYCOMP_ARGPARSER_H

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace myComp {
class ArgParser {
  public:
    void parse(int argc, char **argv);

    bool debug() const { return _debug; }
    bool const_propagation() const { return _const_propagation; }
    const std::string &file_name() const { return _file_name; }
    const std::string &program_name() const { return _program_name; }

  private:
    // Convert c-style string to std::string
    void _stringfy(int argc, char **argv);

    std::vector<std::string> _args;
    bool _debug = false;
    bool _const_propagation = false;
    std::string _file_name;
    std::string _program_name;
};
} // namespace myComp

#endif