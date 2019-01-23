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
#include <SerialPort.h>
#include "KnxTinySerial.h"
#include "Utils.h"
#include "log.h"
#include "Settings.h"
#include <signal.h>

using namespace mylog;

sig_atomic_t is_running = 1;

const size_t loop_ms=100;

void sig_handler (int param)
{
  is_running = 0;
}

int main(int argc, char* argv[]) {

  Settings settings;

  ExitCodes ret = settings.ParseCommandLineArguments(argc, argv);
  if (ret != ExitCodes::Ok) return as_integer(ret);

  if (settings.verbose)
    FILELog::ReportingLevel() = logTRACE;
  else
    FILELog::ReportingLevel() = logINFO;

  void (*prev_handler)(int);
  prev_handler = signal (SIGINT, sig_handler);

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

  KnxTinySerial kdriver(serial_port);

  kdriver.Init();

  while (is_running == 1) {
    std::vector<unsigned char> frame;
    if (kdriver.Read(frame)) {
      PrintMsg(frame);
    }
    else if (frame.size()) {
      FILE_LOG(logWARNING) << "INVALID MESSAGE";
      PrintMsg(frame);
    }
    kdriver.Sleep(loop_ms);
  }

  kdriver.DeInit();

  serial_port.Close();

  FILE_LOG(logINFO) << "done." << std::endl;
  return EXIT_SUCCESS;
}
