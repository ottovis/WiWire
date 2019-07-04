// ==========================================================================
//
// File      : wiwire.cpp
// Part of   : C++ libary WiWire, used for wireless transmission
// Copyright : otto@devisser.dev 2019
//
// Distributed under the GNU GENERAL PUBLIC LICENSE, version 3
// (See accompanying file LICENSE file, or look at https://fsf.org/
//
// ==========================================================================

#include "wiwire.hpp"

int wiwire::send(const char *msg, int sizeMsg, char hwtarget) {
  int attemptCounter = 0;
  char readByte = 0;
  do {
    attemptCounter++;
    if (attemptCounter > retryAmount) {
      return -1;
    }

    sendByte(STARTBYTE);
    sendByte(SENDBYTE);
    sendByte(hwtarget);
    sendByte(sizeMsg);
    for (int i = 0; i < sizeMsg; i++) {
      sendByte(msg[i]);
    }
    sendByte(checksum(msg, sizeMsg));
    sendByte(STOPBYTE);
    readByte = readOneByte();
  } while (readByte != ACKBYTE);
  return attemptCounter;
}

int wiwire::broadcast(const char *msg, const int &sizeMsg) {
  sendByte(STARTBYTE);
  sendByte(BROADCASTBYTE);
  sendByte(sizeMsg);
  for (int i = 0; i < sizeMsg; i++) {
    sendByte(*(msg + i));
  }
  sendByte(checksum(msg, sizeMsg));
  sendByte(STOPBYTE);
  return 0;
}

char wiwire::checksum(const char *msg, const int &sizeMsg) {
  char tmp = *msg;
  if (sizeMsg <= 1) {
    return tmp;
  }
  for (int i = 1; i < sizeMsg; i++) {
    tmp ^= msg[i];
  }
  return tmp;
}

bool wiwire::verify(const char *msg, const int &sizeMsg, const char &byte) {
  char tmp = *msg;
  if (sizeMsg <= 1) {
    return tmp == byte;
  }
  for (int i = 1; i < sizeMsg; i++) {
    tmp ^= msg[i];
  }
  bool result = tmp == byte;
  return result;
}

void wiwire::sendByte(char byte) {
  testProbe.write(1);
  testProbe.flush();
  for (int i = 0; i < 8; i++) {
    if ((byte >> i) % 2) {
      txPin.write(1);
      txPin.flush();
      hwlib::wait_us(1500);
      txPin.write(0);
      txPin.flush();
      hwlib::wait_us(500);
    } else {
      txPin.write(1);
      txPin.flush();
      hwlib::wait_us(500);
      txPin.write(0);
      txPin.flush();
      hwlib::wait_us(1500);
    }
  }
  testProbe.write(0);
  testProbe.flush();
}

char wiwire::readOneByte() {
  char rollingBuffer[10];
  int buffCounter = 0;
  int highLow = 0;
  bool last = false;
  int bitcount = 0;
  int watchdog = 0;
  int bitsRead = 0;
  char returnByte = 0;

  startPin.write(1);
  startPin.flush();
  for (int i = 0; i < 10; i++) {
    rollingBuffer[i] = rxPin.read();
  }
  // testProbe.write(1); testProbe.flush();
  while (bitsRead < 8 && watchdog < 1000) {
    watchdog++;
    testProbe.write(bitsRead % 2);
    testProbe.flush();
    bitcount = 0;
    int readValue = rxPin.read();
    rollingBuffer[buffCounter] = readValue;
    for (int i = 0; i < 10; i++) {
      bitcount += rollingBuffer[i];
    }

    if (buffCounter < 8) {
      buffCounter++;
    } else {
      buffCounter = 0;
    }

    if (bitcount >= 7) {
      highLow++;
      last = true;
    } else {
      if (last) {
        if (highLow < 300) {
          returnByte >>= 1;
          bitsRead++;
          logicLow.write(1);
          logicLow.flush();
          watchdog = 0;
        } else {
          returnByte >>= 1;
          returnByte += 0b10000000;
          bitsRead++;
          logicHigh.write(1);
          logicHigh.flush();
          watchdog = 0;
        }
      }
      highLow = 0;
      last = false;
      hwlib::wait_us(1);
      // testProbe.write(0); testProbe.flush();
      logicHigh.write(0);
      logicHigh.flush();
      logicLow.write(0);
      logicLow.flush();
    }
  }

  startPin.write(0);
  startPin.flush();
  hwlib::wait_us(10);
  return returnByte;
}

void wiwire::findStartByte() {
  while (true) {
    char tmp = readOneByte();
    if (tmp != STARTBYTE) {
      testProbe.write(0);
      testProbe.flush();
      hwlib::wait_us(10);
      // hwlib::cout << "Bad start byte, expected: " << STARTBYTE << " Got: "
      // << tmp << "\n";
      testProbe.write(1);
      testProbe.flush();
    } else {
      // hwlib::cout << "returning\n";
      return;
    }
  }
}

char wiwire::blockRead(char *msg) {
retry:
  while (true) {
    // hwlib::cout << "Retrying" << "\n";
    testProbe.write(1);
    testProbe.flush();
    findStartByte();
    testProbe.write(0);
    testProbe.flush();
    char mode = readOneByte();
    if (mode == SENDBYTE) {
      if (readOneByte() != hwid) {
        continue;
      }
    }
    testProbe.write(1);
    testProbe.flush();
    char sizeMsg = readOneByte();
    testProbe.write(0);
    testProbe.flush();
    for (int i = 0; i < sizeMsg; i++) {
      msg[i] = readOneByte();
      testProbe.write(i % 2);
      testProbe.flush();
    }
    char tmp = readOneByte();
    testProbe.write(1);
    testProbe.flush();
    char tmp2 = tmp;
    if (!verify(msg, sizeMsg, tmp)) {
      hwlib::cout << "failed verify, expected: " << (int)tmp2
                  << " got: " << (int)tmp << "\n";
      if (mode != BROADCASTBYTE) {
        hwlib::wait_ms(10);
        sendByte(RESENDBYTE);
      }
      goto retry;
    }
    testProbe.write(0);
    testProbe.flush();
    char tmp3 = readOneByte();
    if (tmp3 != STOPBYTE) {
      hwlib::cout << "failed stopByte, expected: " << STOPBYTE
                  << " got: " << (int)tmp3 << "\n";
      if (mode != BROADCASTBYTE) {
        hwlib::wait_ms(10);
        sendByte(RESENDBYTE);
      }
      goto retry;
    }
    testProbe.write(1);
    testProbe.flush();
    if (mode == BROADCASTBYTE) {
      return sizeMsg;
    }
    hwlib::wait_ms(10);
    sendByte(ACKBYTE);
    return sizeMsg;
  }
}

void wiwire::setAttempts(int &setAttempts) { retryAmount = setAttempts; }