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

#include "ReadManager.h"
#include "Utils.h"
#include "log.h"
#include <iostream>

const size_t loop_ms=100;

ReadManager::ReadManager(SerialPort& serial_port):
  m_serial_port(serial_port)
{

}

void ReadManager::Start()
{
  Locker locker(m_start_stop_mutex);
  StopUnlocked();
  JoinUnlocked();
  StartUnlocked();
}

void ReadManager::Stop()
{
  StopUnlocked();
}

void ReadManager::Join() {
  Locker locker(m_start_stop_mutex);
  JoinUnlocked();
}

void ReadManager::Loop(SerialPort& serial_port)
{
  KnxTinySerial kdriver(serial_port);
  kdriver.Init();
  while (true) {
    Locker locker(m_actions_mutex);

    if (m_exit) {
      kdriver.DeInit();
      return;
    }
    else {
      std::vector<unsigned char> frame;
      if (kdriver.Read(frame)) {
        PrintMsg(frame);
      }
      else if (frame.size()) {
        std::cout << "INVALID MESSAGE" << std::endl;
        PrintMsg(frame);
      }

// TODO:
//      locker.unlock();
//      kdriver.Sleep(loop_ms);
//      locker.lock();
      m_cond.wait_for(locker, std::chrono::milliseconds(loop_ms));
    }
  }
}

void ReadManager::StartUnlocked() {
  m_exit = false;
  m_thread = std::thread([this] { Loop(m_serial_port); });
}

void ReadManager::StopUnlocked() {
  {
    Locker locker(m_actions_mutex);
    m_exit = true;
  }
  m_cond.notify_one();
}

void ReadManager::JoinUnlocked() {
  if (m_thread.joinable())
    m_thread.join();
}
