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

#ifndef KNXTINYSERIAL_H
#define KNXTINYSERIAL_H

#include <cstdint>
#include <SerialPort.h>

extern const size_t timeout;
extern bool is_running;

class KnxTinySerial
{
public:
  KnxTinySerial(SerialPort& serial_port, const uint16_t individual_addr = 0x110F);
  ~KnxTinySerial();

  void Init();
  void DeInit();

  bool Read(std::vector<uint8_t> &rx_frame);

  static void PrintHexByte(uint8_t byte);
  static void PrintByte(uint8_t byte);
  static void PrintMsg(std::vector<uint8_t>& data);

  void Sleep(const unsigned int ms_timeout = timeout);

private:
  SerialPort& m_serial_port;
  uint16_t m_individual_addr;

  bool CheckChecksum(std::vector<uint8_t> frame, uint8_t checksum);

  bool CheckState();
  void Reset();
  void SetIndividualAddress();

  void Wait(const unsigned int ms_timeout = 500);

  bool SerialReadAndCompare(const std::vector<uint8_t>& compare_buf,
                            const unsigned int ms_timeout = timeout);
  bool SerialReadByte(uint8_t& rx_byte,
                      const unsigned int ms_timeout = timeout);
  void Flush();
};

#endif // KNXTINYSERIAL_H
