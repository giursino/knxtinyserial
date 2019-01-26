/*
Copyright 2018 giursino

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

#include <iostream>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <iomanip>
#include <thread>
#include <signal.h>
#include <SerialPort.h>
#include "KnxTinySerial.h"
#include "Utils.h"
#include "log.h"
#include "Settings.h"
#include "ReadManager.h"

using namespace mylog;

static bool exit_flag = false;
static std::condition_variable exit_cond;
static std::mutex exit_mutex;

void SignalHandler(int s) {
  if (s == SIGINT) {
    std::lock_guard<std::mutex> lock(exit_mutex);
    exit_flag = true;
    exit_cond.notify_one();
  } else {
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char* argv[]) {

  Settings settings;

  ExitCodes ret = settings.ParseCommandLineArguments(argc, argv);
  if (ret != ExitCodes::Ok) return as_integer(ret);

  if (settings.verbose)
    FILELog::ReportingLevel() = logTRACE;
  else
    FILELog::ReportingLevel() = logINFO;

  // Initialize signal handling
  struct sigaction action;
  action.sa_handler = SignalHandler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGINT, &action, nullptr);

  FILE_LOG(logINFO) << "Opening..." << settings.serial;
  SerialPort serial_port(settings.serial);

  try {
    serial_port.Open();
  } catch (std::exception) {
    FILE_LOG(logERROR) << "ERROR: cannot open serial port";
    return as_integer(ExitCodes::GenericError);
  }

  if (serial_port.IsOpen() == false) {
    FILE_LOG(logERROR) << "ERROR: serial port not open!";
    return as_integer(ExitCodes::GenericError);
  }

  ReadManager manager(serial_port);

  manager.Start();

  FILE_LOG(logDEBUG) << "waiting exit...";
  {
    std::unique_lock<std::mutex> lock(exit_mutex);
    exit_cond.wait(lock, [] { return exit_flag;});
  }

  FILE_LOG(logDEBUG) << "exit...";
  manager.Stop();
  manager.Join();
  serial_port.Close();

  FILE_LOG(logINFO) << "done." << std::endl;
  return EXIT_SUCCESS;
}
