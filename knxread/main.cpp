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

using namespace mylog;

//#define SERIAL_PATH "/dev/ttyUSB0"
#define SERIAL_PATH "/dev/serial0"


bool is_running=true;
const size_t loop_ms=100;



int main()
{
  FILE_LOG(logINFO) << "Starting...";

  FILE_LOG(logDEBUG) << "opening..." << SERIAL_PATH;
  SerialPort serial_port( SERIAL_PATH ) ;
  serial_port.Open();

  if (serial_port.IsOpen() == false) {
    FILE_LOG(logWARNING) << "WARNING: serial port not open!";
    return EXIT_FAILURE;
  }

  KnxTinySerial kdriver(serial_port);

  kdriver.Init();

  while (is_running) {
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
