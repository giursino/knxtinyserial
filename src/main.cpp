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

#ifdef ENABLE_SERIAL
#include <SerialPort.h>
#include "KnxTinySerial.h"


//#define SERIAL_PATH "/dev/ttyUSB0"
#define SERIAL_PATH "/dev/serial0"


bool is_running=true;
const size_t timeout=2000;


void SerialInit(SerialPort& serial_port) {

  std::cout << "Init serial..." << std::endl;

  std::cout << "setting baudrate..." << std::endl;
  serial_port.SetBaudRate( SerialPort::BaudRate::BAUD_19200 );
  std::cout << "setting char size..." << std::endl;
  serial_port.SetCharSize( SerialPort::CharacterSize::CHAR_SIZE_8 );
  std::cout << "setting parity..." << std::endl;
  serial_port.SetParity( SerialPort::Parity::PARITY_EVEN );
  std::cout << "setting stop bits..." << std::endl;
  serial_port.SetNumOfStopBits(SerialPort::StopBits::STOP_BITS_1);
  std::cout << "setting hw control..." << std::endl;
  serial_port.SetFlowControl(SerialPort::FlowControl::FLOW_CONTROL_NONE);

  std::cout << "Init done." << std::endl;
}

void SerialPortTest(void)
{
    std::cout << "opening..." << SERIAL_PATH << std::endl;
    SerialPort serial_port( SERIAL_PATH ) ;
    serial_port.Open();

    if (serial_port.IsOpen() == false) {
        std::cout << "WARNING: serial port not open!" << std::endl;
        return;
    }

    SerialInit(serial_port);

    char write_byte_1 = 'a';

    char read_byte_1 = 'A';

    // Write a character.
    serial_port.WriteByte(write_byte_1);

    size_t timeout_milliseconds = 5;

    try
    {
       // Read a character.
       read_byte_1 = serial_port.ReadByte(timeout_milliseconds);

    }
    catch (SerialPort::ReadTimeout)
    {
      std::cerr << "The Read() call has timed out." << std::endl;
    }

    std::cout << "serial_port read:   " << read_byte_1 << std::endl;

    serial_port.Close();
}


void KnxTinySerialTest(void) {
  std::cout << "opening..." << SERIAL_PATH << std::endl;
  SerialPort serial_port( SERIAL_PATH ) ;
  serial_port.Open();

  if (serial_port.IsOpen() == false) {
    std::cout << "WARNING: serial port not open!" << std::endl;
    return;
  }

  KnxTinySerial kdriver(serial_port);

  kdriver.Init();
  kdriver.Read();
  kdriver.DeInit();

  serial_port.Close();
}

#endif


int main()
{
  std::cout << "Starting..." << std::endl;

#ifdef ENABLE_SERIAL
//  SerialPortTest();
    KnxTinySerialTest();
#endif

  std::cout << "done." << std::endl;
  return 0;
}
