/*
Copyright 2019 Giuseppe Ursino

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
#include "Utils.h"
#include <iomanip>
#include <sstream>

void PrintHexByte(uint8_t byte) {
  std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte) << ' ';
}

void PrintByte(uint8_t byte) {
  std::cout << "byte: ";
  PrintHexByte(byte);
  std::cout << std::endl;
}

void PrintMsg(std::vector<uint8_t> &data) {
  if (data.size() != 0) {
    std::cout << "msg: ";
    for (auto i: data)
      PrintHexByte(i);
    std::cout << std::endl;
  }
  std::cout << std::flush;
}


std::string ByteToHexString(const uint8_t& byte){
  std::stringstream str;
  str << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte);
  return str.str();
}

std::string ByteVectorToHexString(const std::vector<uint8_t>& byte_vector){
  std::stringstream str;
  for (auto i: byte_vector) {
    str << ByteToHexString(i) << ' ';
  }
  return str.str();
}

std::vector<uint8_t> HexStringToByteVector(const std::string& hex_string){
  std::vector<uint8_t> bytes;

  for (unsigned int i = 0; i < hex_string.length(); i += 2){
    std::string byte_string = hex_string.substr(i, 2);
    uint8_t byte = static_cast<uint8_t>(strtol(byte_string.c_str(), nullptr, 16));
    bytes.push_back(byte);
  }

  return bytes;
}
