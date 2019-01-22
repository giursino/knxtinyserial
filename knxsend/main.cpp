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
#include "Settings.h"
#include "Utils.h"


bool is_running=true;

bool send(const KnxTinySerial &kdriver, const std::vector<uint8_t> &tx_frame) {
  //send
  kdriver.Write(tx_frame);

  // wait confirmation
  bool success=false;
  while (true) {
    std::vector<uint8_t> rx_frame;
    success = kdriver.Read(rx_frame);

    if (rx_frame.empty()) {
      std::cout << "timeout" << std::endl;
      success=false;
      break;
    }

    PrintMsg(rx_frame);
    if ((!success) && (rx_frame[0] == 0x8B)) {
      std::cout << "send: OK" << std::endl;
      success=true;
      break;
    }
    else if ((!success) && (rx_frame[0] == 0x0B)) {
      std::cout << "send: ERROR" << std::endl;
      success=false;
      break;
    }
  }
  return success;
}

int main(int argc, char* argv[]) {

  Settings settings;

  ExitCodes ret = settings.ParseCommandLineArguments(argc, argv);
  if (ret != ExitCodes::Ok) return as_integer(ret);

  std::cout << "opening..." << settings.serial << std::endl;
  SerialPort serial_port(settings.serial);

  try {
    serial_port.Open();
  } catch (std::exception) {
    std::cout << "ERROR: cannot open serial port" << std::endl;
    return as_integer(ExitCodes::GenericError);
  }

  if (serial_port.IsOpen() == false) {
    std::cout << "ERROR: serial port not open!" << std::endl;
    return as_integer(ExitCodes::GenericError);
  }

  KnxTinySerial kdriver(serial_port);

  kdriver.Init();

  std::cout << "Send message: " << ByteVectorToHexString(settings.tx_message) << std::endl;

  bool success = send(kdriver, settings.tx_message);

  kdriver.DeInit();

  serial_port.Close();

  std::cout << "done." << std::endl;

  if (success)
    return as_integer(ExitCodes::Ok);
  else
    return  as_integer(ExitCodes::GenericError);
}
