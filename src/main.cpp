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

void PrintHexByte(unsigned char byte) {
  std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte) << ' ';
}

void PrintByte(unsigned char byte) {
  std::cout << "Read: ";
  PrintHexByte(byte);
  std::cout << std::endl;
}

void PrintMsg(std::vector<unsigned char> &data) {
  if (data.size() != 0) {
    std::cout << "Read: ";
    for (auto i: data)
      PrintHexByte(i);
    std::cout << std::endl;
  }
  std::cout << std::flush;
}

void SerialReadBlocking(SerialPort& serial_port, const unsigned int msTimeout, std::vector<unsigned char> &buffer) {
  std::this_thread::sleep_for(std::chrono::milliseconds(msTimeout));
  return serial_port.Read(buffer);
}

bool TinySerialCheckState(SerialPort& serial_port) {
  std::cout << "*** checking state procedure..." << std::endl;
  std::cout << "sending U_State.req..." << std::endl;
  serial_port.WriteByte(0x02);
  std::cout << "waiting..." << std::endl;
  try {
    unsigned char rxbyte = serial_port.ReadByte(timeout);
    PrintByte(rxbyte);
    if (rxbyte == 0x07) {
      return true;
    }
  }
  catch (SerialPort::ReadTimeout) {
    std::cout << "no response" << std::endl;
  }
  std::cout << "error state" << std::endl;
  return false;
}

void TinySerialReset(SerialPort& serial_port) {
  bool retry = true;

  while(retry) {
    std::cout << "*** starting reset procedure..." << std::endl;
    std::cout << "sending U_Reset.req..." << std::endl;
    serial_port.WriteByte(0x01);
    std::cout << "waiting U_Reset.ind (5s timeout)..." << std::endl;
    try {
      unsigned char rxbyte = serial_port.ReadByte(5000);
      PrintByte(rxbyte);
      if (rxbyte == 0x03) {
        retry = false;
      }
      else {
        std::cout << "invalid response" << std::endl;
      }
    }
    catch (SerialPort::ReadTimeout)
    {
      std::cerr << "The Read() call has timed out." << std::endl;
    }
  }

}


void TinySerialSetIndividualAddress(SerialPort& serial_port, unsigned char addr_high, unsigned char addr_low) {
  std::vector<unsigned char> rxbuf;
  bool retry = true;

  while(retry) {
    std::cout << "*** setting individual address procedure..." << std::endl;;
    try {

      std::cout << "sending U_WriteProperty.req to disable AUTO IACK..." << std::endl;
      serial_port.WriteByte(0x22);
      serial_port.WriteByte(0x00);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      std::cout << "sending U_ReadProperty.req..." << std::endl;
      serial_port.WriteByte(0x1A);
      std::cout << "waiting..." << std::endl;
      rxbuf.clear();
      serial_port.Read(rxbuf, 2, timeout);
      PrintMsg(rxbuf);
      if (rxbuf.size() != 0) {
        if ((rxbuf[0] != 0x1A) || (rxbuf[1] != 0x00)) {
          std::cout << "response invalid" << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
          continue;
        }
      }

      std::cout << "sending U_WriteAHigh.req..." << std::endl;
      serial_port.WriteByte(0x1F);
      serial_port.WriteByte(addr_high);

      std::cout << "sending U_WriteALow.req..." << std::endl;
      serial_port.WriteByte(0x1E);
      serial_port.WriteByte(addr_low);

      std::cout << "sending U_WriteProperty.req to enable AUTO IACK..." << std::endl;
      serial_port.WriteByte(0x22);
      serial_port.WriteByte(0x01);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      std::cout << "sending U_ReadProperty.req..." << std::endl;
      serial_port.WriteByte(0x1A);
      std::cout << "waiting..." << std::endl;
      rxbuf.clear();
      serial_port.Read(rxbuf, 2, timeout);
      PrintMsg(rxbuf);
      if (rxbuf.size() != 0) {
        if ((rxbuf[0] != 0x1A) || (rxbuf[1] != 0x01)) {
          std::cout << "response invalid" << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
          continue;
        }
      }

      retry = false;
    }
    catch (SerialPort::ReadTimeout)
    {
      std::cerr << "The Read() call has timed out." << std::endl;
    }
  }

}

void TinySerialInit(SerialPort& serial_port) {
  bool retry = true;

  std::cout << "init Tiny KNX Serial..." << std::endl;
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


  while (retry) {
    TinySerialReset(serial_port);
    TinySerialSetIndividualAddress(serial_port, 0x11, 0x0F);
    if (TinySerialCheckState(serial_port) == false) {
      std::cerr << "error checking state" << std::endl;
      continue;
    }
    retry = false;
  }

  std::cout << "init done." << std::endl;
}



std::vector<unsigned char> TinySerialRead(SerialPort& serial_port) {
  std::vector<unsigned char> buf;
  size_t timeout_milliseconds = 100;

  try
  {
    serial_port.Read(buf, 0, timeout_milliseconds);
  }
  catch (SerialPort::ReadTimeout)
  {
    std::cerr << "The Read() call has timed out." << std::endl;
  }

  return buf;
}


void TinySerialTest(void) {
  std::cout << "opening..." << SERIAL_PATH << std::endl;
  SerialPort serial_port( SERIAL_PATH ) ;
  serial_port.Open();

  if (serial_port.IsOpen() == false) {
      std::cout << "WARNING: serial port not open!" << std::endl;
      return;
  }

  TinySerialInit(serial_port);

  std::vector<unsigned char> data;

  while (is_running) {
    //cout << "Reading..." << endl;
    data = TinySerialRead(serial_port);
    PrintMsg(data);

    //std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  serial_port.Close();
}


std::vector<unsigned char> TinySerialReadMsg(SerialPort& serial_port) {
  std::vector<unsigned char> buf;
  size_t timeout_milliseconds = 5;

  try
  {
//    while(serial_port.IsDataAvailable()) {
    while(true) {

      buf.push_back(serial_port.ReadByte(timeout_milliseconds));
    }
  }
  catch (SerialPort::ReadTimeout)
  {
    std::cerr << "The Read() call has timed out." << std::endl;
  }

  return buf;
}


void TinySerialTest2(void) {
  std::cout << "opening..." << SERIAL_PATH << std::endl;
  SerialPort serial_port( SERIAL_PATH ) ;
  serial_port.Open();

  if (serial_port.IsOpen() == false) {
    std::cout << "WARNING: serial port not open!" << std::endl;
    return;
  }

  TinySerialInit(serial_port);

  std::vector<unsigned char> data;

  while (is_running) {

//    cout << "Reading..." << endl;

    if (serial_port.IsDataAvailable()) {
      std::cout << "Data available..." << std::endl;
      data = TinySerialReadMsg(serial_port);
      PrintMsg(data);

    }
//    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  serial_port.Close();
}


void TinySerialTest3(void) {
  std::cout << "opening..." << SERIAL_PATH << std::endl;
  SerialPort serial_port( SERIAL_PATH ) ;
  serial_port.Open();

  if (serial_port.IsOpen() == false) {
    std::cout << "WARNING: serial port not open!" << std::endl;
    return;
  }

  TinySerialInit(serial_port);

  while (is_running) {
    unsigned char buf;

    buf = serial_port.ReadByte(0);

    if (buf == 0xbc) {
      std::cout << std::endl;
      std::cout << "Read: ";
    }
    PrintHexByte(buf);
  }

  serial_port.Close();
}

void TinySerialTest4(void) {
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


void SerialPortTest(void)
{
    std::cout << "opening..." << SERIAL_PATH << std::endl;
    SerialPort serial_port( SERIAL_PATH ) ;
    serial_port.Open();

    if (serial_port.IsOpen() == false) {
        std::cout << "WARNING: serial port not open!" << std::endl;
        return;
    }

    TinySerialInit(serial_port);

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
#endif


int main()
{
  std::cout << "Starting..." << std::endl;

#ifdef ENABLE_SERIAL
//  SerialPortTest();
//  TinySerialTest();
//  TinySerialTest2();
//  TinySerialTest3();
    TinySerialTest4();
#endif

  std::cout << "done." << std::endl;
  return 0;
}
