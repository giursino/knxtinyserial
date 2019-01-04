/*
Copyright 2019 giursino

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
#ifdef ENABLE_SERIAL

#include <iostream>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <iomanip>
#include "KnxTinySerial.h"

void KnxTinySerial::PrintHexByte(uint8_t byte) {
  std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte) << ' ';
}

void KnxTinySerial::PrintByte(uint8_t byte) {
  std::cout << "Read: ";
  PrintHexByte(byte);
  std::cout << std::endl;
}

void KnxTinySerial::PrintMsg(std::vector<uint8_t> &data) {
  if (data.size() != 0) {
    std::cout << "Read: ";
    for (auto i: data)
      PrintHexByte(i);
    std::cout << std::endl;
  }
  std::cout << std::flush;
}

void KnxTinySerial::Sleep(const unsigned int ms_timeout)
{
  if (!m_serial_port.IsDataAvailable()) Wait(ms_timeout);
}

bool KnxTinySerial::SerialReadAndCompare(const std::vector<uint8_t> &compare_buf,
                                         const unsigned int ms_timeout)
{
  std::vector<uint8_t> receive_buf;
  try {
    m_serial_port.Read(receive_buf, compare_buf.size(), ms_timeout);
    PrintMsg(receive_buf);
    if (receive_buf == compare_buf) {
      return true;
    }
  }
  catch (SerialPort::ReadTimeout){
    std::cout << "no response" << std::endl;
  }

  return false;
}

bool KnxTinySerial::SerialReadByte(uint8_t &rx_byte, const unsigned int ms_timeout) {
  try {
    rx_byte = m_serial_port.ReadByte(ms_timeout);
    return true;
  }
  catch (SerialPort::ReadTimeout){}
  return false;
}

void KnxTinySerial::Wait(const unsigned int ms_timeout)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms_timeout));
}

void KnxTinySerial::Flush()
{
  try {
    std::vector<uint8_t> buf;
    m_serial_port.Read(buf, 0, timeout);
  }
  catch (SerialPort::ReadTimeout){}
}


bool KnxTinySerial::CheckState() {
  std::cout << "*** checking state procedure..." << std::endl;
  std::cout << "sending U_State.req..." << std::endl;
  m_serial_port.WriteByte(0x02);
  std::cout << "waiting..." << std::endl;
  if (SerialReadAndCompare({0x07})) {
      return true;
  }
  std::cout << "error state" << std::endl;
  return false;
}


void KnxTinySerial::Reset() {
  bool retry = true;

  while(retry) {
    std::cout << "*** starting reset procedure..." << std::endl;
    std::cout << "sending U_Reset.req..." << std::endl;
    m_serial_port.WriteByte(0x01);
    std::cout << "waiting U_Reset.ind (5s timeout)..." << std::endl;
    if (SerialReadAndCompare({0x03}, 5000)) {
        retry = false;
    }
    else {
      std::cout << "invalid response" << std::endl;
    }
  }

}

void KnxTinySerial::SetIndividualAddress() {
  bool retry = true;
  uint8_t addr_high = (m_individual_addr & 0xFF00) >> 8;
  uint8_t addr_low = (m_individual_addr & 0x00FF);

  while(retry) {
    std::cout << "*** setting individual address procedure..." << std::endl;;
    Wait();
    Flush();

    std::cout << "sending U_WriteProperty.req to disable AUTO IACK..." << std::endl;
    m_serial_port.WriteByte(0x22);
    m_serial_port.WriteByte(0x00);

    std::cout << "sending U_ReadProperty.req..." << std::endl;
    m_serial_port.WriteByte(0x1A);
    std::cout << "waiting..." << std::endl;
    if (!SerialReadAndCompare({0x1A, 0x00})) {
      std::cout << "response invalid" << std::endl;
      continue;
    }

    std::cout << "sending U_WriteAHigh.req..." << std::endl;
    m_serial_port.WriteByte(0x1F);
    m_serial_port.WriteByte(addr_high);

    std::cout << "sending U_WriteALow.req..." << std::endl;
    m_serial_port.WriteByte(0x1E);
    m_serial_port.WriteByte(addr_low);

    std::cout << "sending U_WriteProperty.req to enable AUTO IACK..." << std::endl;
    m_serial_port.WriteByte(0x22);
    m_serial_port.WriteByte(0x01);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "sending U_ReadProperty.req..." << std::endl;
    m_serial_port.WriteByte(0x1A);
    std::cout << "waiting..." << std::endl;
    if (!SerialReadAndCompare({0x1A, 0x01})) {
      std::cout << "response invalid" << std::endl;
      continue;
    }

    retry = false;
  }

}

KnxTinySerial::KnxTinySerial(SerialPort& serial_port, const uint16_t individual_addr):
  m_serial_port(serial_port), m_individual_addr(individual_addr)
{

}

KnxTinySerial::~KnxTinySerial()
{
}

void KnxTinySerial::Init() {
  bool retry = true;

  if (m_serial_port.IsOpen() == false) {
    std::cerr << "WARNING: serial port not open!" << std::endl;
    return;
  }

  std::cout << "Init KNX Tiny Serial..." << std::endl;
  std::cout << "setting baudrate..." << std::endl;
  m_serial_port.SetBaudRate( SerialPort::BaudRate::BAUD_19200 );
  std::cout << "setting char size..." << std::endl;
  m_serial_port.SetCharSize( SerialPort::CharacterSize::CHAR_SIZE_8 );
  std::cout << "setting parity..." << std::endl;
  m_serial_port.SetParity( SerialPort::Parity::PARITY_EVEN );
  std::cout << "setting stop bits..." << std::endl;
  m_serial_port.SetNumOfStopBits(SerialPort::StopBits::STOP_BITS_1);
  std::cout << "setting hw control..." << std::endl;
  m_serial_port.SetFlowControl(SerialPort::FlowControl::FLOW_CONTROL_NONE);


  while (retry) {
    Reset();
    SetIndividualAddress();
    if (CheckState() == false) {
      std::cerr << "error checking state" << std::endl;
      continue;
    }
    retry = false;
  }

  std::cout << "Init done." << std::endl;
}

void KnxTinySerial::DeInit()
{
}


bool KnxTinySerial::CheckChecksum(std::vector<uint8_t> frame, uint8_t checksum) {
  uint8_t cs = 0xFF;
  for (auto i: frame) cs ^= i;
  cs ^= checksum;
  return !cs ? true : false;
}

bool KnxTinySerial::Read(std::vector<uint8_t> &rx_frame) {
  const int timeout = 500;
  uint8_t rx_byte;

  rx_frame.clear();

  // control field or UART service
  if (!SerialReadByte(rx_byte, timeout)) return false;
  if (rx_byte == 0x03) {
    // UART_Reset.ind
    SetIndividualAddress();
    return false;
  }
  if (rx_byte == 0x0B) {
    // TODO: conferma invio messaggio, da sincronizzare
    std::cout << "L_DATA.conf: negative" << std::endl;
    return false;
  }
  if (rx_byte == 0x8B) {
    // TODO: conferma invio messaggio, da sincronizzare
    std::cout << "L_DATA.conf: positive" << std::endl;
    return false;
  }
  if ((rx_byte & 0xD3) != 0x90) return false;
  rx_frame.push_back(rx_byte);

  // source address
  if (!SerialReadByte(rx_byte, timeout)) return false;
  rx_frame.push_back(rx_byte);
  if (!SerialReadByte(rx_byte, timeout)) return false;
  rx_frame.push_back(rx_byte);

  // destination address
  if (!SerialReadByte(rx_byte, timeout)) return false;
  rx_frame.push_back(rx_byte);
  if (!SerialReadByte(rx_byte, timeout)) return false;
  rx_frame.push_back(rx_byte);

  // DAF & length
  if (!SerialReadByte(rx_byte, timeout)) return false;
  rx_frame.push_back(rx_byte);
  uint8_t length = (rx_byte & 0x0F) + 1;

  // payload
  for (uint8_t i=0; i<length; i++) {
    if (!SerialReadByte(rx_byte, timeout)) return false;
    rx_frame.push_back(rx_byte);
  }

  // checksum
  if (!SerialReadByte(rx_byte, timeout)) return false;
  if (!CheckChecksum(rx_frame, rx_byte)) return false;

  return true;
}

#endif
