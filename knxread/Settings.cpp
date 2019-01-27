/*
Copyright 2018 Giuseppe Ursino

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "Settings.h"

#include "Config.h"
#include "Utils.h"
#include <boost/program_options.hpp>
#include <functional>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace boost::program_options;

extern const std::map<ExitCodes, std::string> exit_codes_help_display = {
  {ExitCodes::Ok, "Ok, no errors"},
  {ExitCodes::GenericError, "Generic error"},
  {ExitCodes::InvalidArgument, "Invalid argument"},
  {ExitCodes::EarlyExit, "Early exit"},
};


void Settings::PrintHelp(options_description &generalOptions) {
  // TODO: program name missing from cmake
//  std::cout << "Usage: " << TARGET_NAME << "" << std::endl;
  std::cout << generalOptions << std::endl;
  std::cout << "Exit status: " << std::endl;
  for(auto& elem : exit_codes_help_display) {
    std::cout << " " << as_integer(elem.first) << ": " << elem.second << std::endl;
  }
}


ExitCodes Settings::ParseCommandLineArguments(int argc, char* argv[]) {

  try {

    options_description generalOptions{"Options"};
    generalOptions.add_options()
        ("help,h", "Help Screen")
        ("verbose,v", bool_switch(&verbose)->default_value(false), "Verbose")
        ("version", "Print version and exit")
        ("serial,s", value<std::string>(&serial)->default_value("/dev/serial0"), "Serial port")
        ;

    variables_map vm;
    store(parse_command_line(argc, argv, generalOptions), vm);
    notify(vm);

    if (vm.count("help")) {
      PrintHelp(generalOptions);
      return ExitCodes::EarlyExit;
    }

    if (vm.count("version")) {
      std::cout << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << "-" << GIT_REVISION << std::endl;
      return ExitCodes::EarlyExit;
    }

    if (verbose) {
      std::cout << "Settings: " << std::endl;
      std::cout << "--------- " << std::endl;
      std::cout << "verbose: " << verbose << std::endl;
      std::cout << "serial: " << serial << std::endl;
      std::cout << "--------- " << std::endl;
    }

  } catch (std::exception& e)  {
    std::cerr << e.what() << std::endl;
    return ExitCodes::GenericError;
  }
  return ExitCodes::Ok;
}
