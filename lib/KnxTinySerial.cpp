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
#include <iostream>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <iomanip>
#include "KnxTinySerial.h"
#include "log.h"

using namespace mylog;

const int read_timeout = 500;
const size_t timeout = 2000;

void KnxTinySerial::Sleep(const unsigned int ms_timeout) const
{
  if (!m_serial_port.IsDataAvailable()) Wait(ms_timeout);
}

bool KnxTinySerial::SerialReadAndCompare(const std::vector<uint8_t> &compare_buf,
                                         const unsigned int ms_timeout) const
{
  std::vector<uint8_t> receive_buf;
  try {
    m_serial_port.Read(receive_buf, compare_buf.size(), ms_timeout);
    if (receive_buf == compare_buf) {
      return true;
    }
  }
  catch (SerialPort::ReadTimeout){
    FILE_LOG(logDEBUG) << "no response";
  }

  return false;
}

bool KnxTinySerial::SerialReadByte(uint8_t &rx_byte, const unsigned int ms_timeout) const {
  try {
    rx_byte = m_serial_port.ReadByte(ms_timeout);
    return true;
  }
  catch (SerialPort::ReadTimeout){}
  return false;
}

void KnxTinySerial::Wait(const unsigned int ms_timeout) const
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

bool KnxTinySerial::CheckState() const{
  FILE_LOG(logDEBUG) << "*** checking state procedure...";
  FILE_LOG(logDEBUG) << "sending U_State.req...";
  m_serial_port.WriteByte(0x02);
  FILE_LOG(logDEBUG) << "waiting...";
  if (SerialReadAndCompare({0x07})) {
      return true;
  }
  FILE_LOG(logDEBUG) << "error state";
  return false;
}


void KnxTinySerial::Reset() const {
  bool retry = true;

  while(retry) {
    FILE_LOG(logDEBUG) << "*** starting reset procedure...";
    FILE_LOG(logDEBUG) << "sending U_Reset.req...";
    m_serial_port.WriteByte(0x01);
    FILE_LOG(logDEBUG) << "waiting U_Reset.ind (5s timeout)...";
    if (SerialReadAndCompare({0x03}, 5000)) {
        retry = false;
    }
    else {
      FILE_LOG(logDEBUG) << "invalid response";
    }
  }

}

void KnxTinySerial::SetIndividualAddress() {
  bool retry = true;
  uint8_t addr_high = (m_individual_addr & 0xFF00) >> 8;
  uint8_t addr_low = (m_individual_addr & 0x00FF);

  while(retry) {
    FILE_LOG(logDEBUG) << "*** setting individual address procedure...";;
    Wait();
    Flush();

    FILE_LOG(logDEBUG) << "sending U_WriteProperty.req to disable AUTO IACK...";
    m_serial_port.WriteByte(0x22);
    m_serial_port.WriteByte(0x00);

    FILE_LOG(logDEBUG) << "sending U_ReadProperty.req...";
    m_serial_port.WriteByte(0x1A);
    FILE_LOG(logDEBUG) << "waiting...";
    if (!SerialReadAndCompare({0x1A, 0x00})) {
      FILE_LOG(logDEBUG) << "response invalid";
      continue;
    }

    FILE_LOG(logDEBUG) << "sending U_WriteAHigh.req...";
    m_serial_port.WriteByte(0x1F);
    m_serial_port.WriteByte(addr_high);

    FILE_LOG(logDEBUG) << "sending U_WriteALow.req...";
    m_serial_port.WriteByte(0x1E);
    m_serial_port.WriteByte(addr_low);

    FILE_LOG(logDEBUG) << "sending U_WriteProperty.req to enable AUTO IACK...";
    m_serial_port.WriteByte(0x22);
    m_serial_port.WriteByte(0x01);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    FILE_LOG(logDEBUG) << "sending U_ReadProperty.req...";
    m_serial_port.WriteByte(0x1A);
    FILE_LOG(logDEBUG) << "waiting...";
    if (!SerialReadAndCompare({0x1A, 0x01})) {
      FILE_LOG(logDEBUG) << "response invalid";
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
    FILE_LOG(logDEBUG) << "WARNING: serial port not open!";
    return;
  }

  FILE_LOG(logDEBUG) << "Init KNX Tiny Serial...";
  FILE_LOG(logDEBUG) << "setting baudrate...";
  m_serial_port.SetBaudRate( SerialPort::BaudRate::BAUD_19200 );
  FILE_LOG(logDEBUG) << "setting char size...";
  m_serial_port.SetCharSize( SerialPort::CharacterSize::CHAR_SIZE_8 );
  FILE_LOG(logDEBUG) << "setting parity...";
  m_serial_port.SetParity( SerialPort::Parity::PARITY_EVEN );
  FILE_LOG(logDEBUG) << "setting stop bits...";
  m_serial_port.SetNumOfStopBits(SerialPort::StopBits::STOP_BITS_1);
  FILE_LOG(logDEBUG) << "setting hw control...";
  m_serial_port.SetFlowControl(SerialPort::FlowControl::FLOW_CONTROL_NONE);


  while (retry) {
    Reset();
    SetIndividualAddress();
    if (CheckState() == false) {
      FILE_LOG(logDEBUG) << "error checking state";
      continue;
    }
    retry = false;
  }

  FILE_LOG(logDEBUG) << "Init done.";
}

void KnxTinySerial::DeInit()
{
}


uint8_t KnxTinySerial::CalculateChecksum(std::vector<uint8_t> frame) const {
  uint8_t cs = 0xFF;
  for (auto i: frame) cs ^= i;
  return cs;
}

bool KnxTinySerial::CheckChecksum(std::vector<uint8_t> frame, uint8_t checksum) const {
  uint8_t cs = CalculateChecksum(frame);
  cs ^= checksum;
  return !cs ? true : false;
}

bool KnxTinySerial::Read(std::vector<uint8_t> &rx_frame) const {
  uint8_t rx_byte;

  rx_frame.clear();

  // control field or UART service
  if (!SerialReadByte(rx_byte, read_timeout)) return false;
  if (rx_byte == 0x03) {
    // UART_Reset.ind
    FILE_LOG(logDEBUG) << "Bus connection lost";
    FILE_LOG(logDEBUG) << "state: " << CheckState();
    return false;
  }
  if (rx_byte == 0x0B) {
    rx_frame.push_back(rx_byte);
    FILE_LOG(logDEBUG) << "L_DATA.conf: negative";
    return false;
  }
  if (rx_byte == 0x8B) {
    rx_frame.push_back(rx_byte);
    FILE_LOG(logDEBUG) << "L_DATA.conf: positive";
    return false;
  }
  if ((rx_byte & 0xD3) != 0x90) return false;
  rx_frame.push_back(rx_byte);

  // source address
  if (!SerialReadByte(rx_byte, read_timeout)) return false;
  rx_frame.push_back(rx_byte);
  if (!SerialReadByte(rx_byte, read_timeout)) return false;
  rx_frame.push_back(rx_byte);

  // destination address
  if (!SerialReadByte(rx_byte, read_timeout)) return false;
  rx_frame.push_back(rx_byte);
  if (!SerialReadByte(rx_byte, read_timeout)) return false;
  rx_frame.push_back(rx_byte);

  // DAF & length
  if (!SerialReadByte(rx_byte, read_timeout)) return false;
  rx_frame.push_back(rx_byte);
  uint8_t length = (rx_byte & 0x0F) + 1;

  // payload
  for (uint8_t i=0; i<length; i++) {
    if (!SerialReadByte(rx_byte, read_timeout)) return false;
    rx_frame.push_back(rx_byte);
  }

  // checksum
  if (!SerialReadByte(rx_byte, read_timeout)) return false;
  if (!CheckChecksum(rx_frame, rx_byte)) return false;

  return true;
}

bool KnxTinySerial::Write(const std::vector<uint8_t>& tx_frame) const
{
  if (tx_frame.size() < 7) return false;

  uint8_t byte_prefix = 0x80;
  for (auto byte_to_send: tx_frame) {
    m_serial_port.WriteByte(byte_prefix++);
    m_serial_port.WriteByte(byte_to_send);
  }

  uint8_t byte_end = 0x40 + static_cast<uint8_t>(tx_frame.size());
  m_serial_port.WriteByte(byte_end);
  uint8_t byte_checksum = CalculateChecksum(tx_frame);
  m_serial_port.WriteByte(byte_checksum);

  return true;
}
