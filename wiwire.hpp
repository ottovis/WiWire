// ==========================================================================
//
// File      : wiwire.hpp
// Part of   : C++ libary WiWire, used for wireless transmission
// Copyright : otto@devisser.dev 2019
//
// Distributed under the GNU GENERAL PUBLIC LICENSE, version 3
// (See accompanying file LICENSE file, or look at https://fsf.org/
//
// ==========================================================================

/// \mainpage WiWire Documentation
/// \section Autor
/// Otto de Visser, first year student at Hogeschool Utrecht
/// Contact at otto@devisser.dev
/// \version
/// Version 0.1, Pre-alpha (last modified at 05-07-2019)
/// \section Copyright
/// GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007

#ifndef WIWIRE_HPP
#define WIWIRE_HPP

#include <stdio.h>
#include <stdlib.h>
#include "hwlib.hpp"

#define STARTBYTE 0b11011011
#define STOPBYTE 0b10001001
#define BROADCASTBYTE 0b00000011
#define SENDBYTE 0b00001100
#define ACKBYTE 0b00110000
#define RESENDBYTE 0b11000000

/// This project uses https://github.com/wovo/hwlib to send and recieve data
/// point to point. This project is made to be used with very basic 433MHz
/// senders and recievers, but there is no inherent problem that limits its
/// use to this only. the basic idea was to make a more robust replacement
/// for a protocol like I2C, with the capability to verify send message,
/// request resends and confirm send messages have been recieved in good
/// health.

/// for more info look @: https://github.com/ottovis/WiWire

class wiwire {
 private:
  /// The pin the transciever is contected to
  hwlib::target::pin_out txPin;

  /// The pin the reciever is contected to
  hwlib::target::pin_in rxPin;

  /// Private function to send one byte given by passing a single char along
  void sendByte(const char byte);

  /// Private function that holds the execution of the program untill a start
  /// byte has been found.
  void findStartByte();

  /// Private function that verifys a given msg using the given char byte.
  /// It does this by XORing the entire message into a single byte and check
  /// that against the given byte.
  bool verify(const char *msg, const int &sizeMsg, const char &byte);

  /// Private function that generates and returns a verifcation byte used to
  /// sign a message. It does this by XORing the entire message into a single
  /// byte and returning that byte
  char checksum(const char *msg, const int &sizeMsg);

  /// Private function that reads exactly one byte of data, if no byte passes by
  /// in a set time, it assumes a wrong timing and returns the thus far read
  /// data. This data should be automaticly discarded by the effects of the
  /// mistiming and the fact that it should not match the verifcation, start and
  /// stop byte.
  char readOneByte();

  /// The given hardware ID of the device. This is used to find targeted
  /// messages
  const char hwid;

  /// Multiple test pins used during development of the project.
  hwlib::target::pin_out testProbe;
  hwlib::target::pin_out logicHigh;
  hwlib::target::pin_out logicLow;
  hwlib::target::pin_out startPin;

  /// amount of allowed retries sending messages
  int retryAmount;

 public:
  ///  Only constructor of the class, takes a transciever pin, a reciever pin
  ///  and a hardware ID and saves them in the object storage
  wiwire(hwlib::target::pin_out txPin, hwlib::target::pin_in rxPin, char hwid)
      : txPin(txPin),
        rxPin(rxPin),
        hwid(hwid),
        testProbe(hwlib::target::pin_out(hwlib::target::pins::d46)),
        logicHigh(hwlib::target::pin_out(hwlib::target::pins::d30)),
        logicLow(hwlib::target::pin_out(hwlib::target::pins::d31)),
        startPin(hwlib::target::pin_out(hwlib::target::pins::d29)){};

  /// Public function that can be used to send data to any avalable devices that
  /// are ready to recieve data. Does not expect a response and does not listen
  /// for repeats or acks. Maximum size of payload is 255 bytes
  int broadcast(const char *msg, const int &sizeMsg);

  /// Public function that can be used to send data to one specific target,
  /// denoted by the target char. Repeats its message untill target acknowledges
  /// message or 8 attempts fail. Returns -1 on failure, or an int with the
  /// amount of attempts before acknowlegement. Maximum size of payload is 255
  /// bytes
  int send(const char *msg, int sizeMsg, char hwtarget);

  /// reads a message and returns the size of the message, use this to read a
  /// save size into msg. This function blocks execution untill a valid message
  /// is recieved. Maximum size of read message is 255
  char blockRead(char *msg);

  /// sets the amount of times a message may be send before it returns a
  /// failure, default is 8.
  void setAttempts(int &setAttempts);
};

#endif  // WIWIRE_HPP