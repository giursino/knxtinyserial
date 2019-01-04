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

#include <SerialPort.h>

extern const size_t timeout;
extern bool is_running;

class KnxTinySerial
{
public:
  KnxTinySerial(SerialPort& serial_port);
  ~KnxTinySerial();

  void Init();
  void DeInit();

  bool Read(std::vector<unsigned char> &rx_frame);

  static void PrintHexByte(unsigned char byte);
  static void PrintByte(unsigned char byte);
  static void PrintMsg(std::vector<unsigned char>& data);

private:
  SerialPort& m_serial_port;


  bool CheckChecksum(std::vector<unsigned char> frame, unsigned char checksum);

  bool CheckState();
  void Reset();
  void SetIndividualAddress(unsigned char addr_high, unsigned char addr_low);

  void Wait();

  bool SerialReadAndCompare(const std::vector<unsigned char>& compare_buf,
                            const unsigned int ms_timeout = timeout);
  bool SerialReadByte(unsigned char& rx_byte,
                      const unsigned int ms_timeout = timeout);
  void Flush();
};

#endif // KNXTINYSERIAL_H
