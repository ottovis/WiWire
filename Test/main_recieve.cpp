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

// Example code for testing and demo purposes
// this code recieves a hello world message at adress 0x0F. When it recieves an
// valid message it will send an ack. The recieved message including its size
// will be printed to the serial port.

int main() {
  namespace target = hwlib::target;

  auto tx = target::pin_out(target::pins::d50);
  auto rx = target::pin_in(target::pins::d51);
  hwlib::wait_ms(1000);

  wiwire wire(tx, rx);
  hwlib::cout << "init"
              << "\n";

  while (true) {
    char msg[256] = {};
    char sizeMsg = wire.blockRead(msg);
    hwlib::cout << "sizeMsg: " << (int)sizeMsg << '\n';
    for (int i = 0; i < sizeMsg; i++) {
      hwlib::cout << msg[i];
    }
    hwlib::cout << "\n";
  }
  return 0;
}