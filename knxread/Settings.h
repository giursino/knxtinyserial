#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <map>
#include <boost/program_options.hpp>

template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

enum class ExitCodes {
  Ok,
  GenericError,
  InvalidArgument,
  EarlyExit,
};

extern const std::map<ExitCodes, std::string> exit_codes_help_display;

class Settings {
public:
  Settings(){;}
  ~Settings(){;}

  ExitCodes ParseCommandLineArguments(int argc, char* argv[]);

  bool verbose;
  std::string serial;

private:
  void PrintHelp(boost::program_options::options_description &generalOptions);
};


#endif // SETTINGS_H
