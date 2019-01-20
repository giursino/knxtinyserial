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


//#define SERIAL_PATH "/dev/ttyUSB0"
#define SERIAL_PATH "/dev/serial0"


bool is_running=true;


int main()
{
  std::cout << "Starting..." << std::endl;

  std::cout << "opening..." << SERIAL_PATH << std::endl;
  SerialPort serial_port( SERIAL_PATH ) ;
  serial_port.Open();

  if (serial_port.IsOpen() == false) {
    std::cout << "WARNING: serial port not open!" << std::endl;
    return EXIT_FAILURE;
  }

  KnxTinySerial kdriver(serial_port);

  kdriver.Init();

  uint8_t data = 0;
  while (is_running) {
    std::cout << "Send message" << std::endl;
    data ^= 1;
    uint8_t data_to_send = 0x80 + data;
    kdriver.Write({0xbc, 0x20, 0x03, 0x21, 0x69, 0xe1, 0x00, data_to_send});
    std::cout << "sleeping..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  kdriver.DeInit();

  serial_port.Close();

  std::cout << "done." << std::endl;
  return EXIT_SUCCESS;
}
