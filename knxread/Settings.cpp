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
