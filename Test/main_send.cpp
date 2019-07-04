// ==========================================================================
//
// File      : main.cpp
// Part of   : C++ libary WiWire, used for wireless transmission
// Copyright : otto@devisser.dev 2019
//
// Distributed under the GNU GENERAL PUBLIC LICENSE, version 3
// (See accompanying file LICENSE file, or look at https://fsf.org/
//
// ==========================================================================

// Example code for testing and demo purposes
// this code sends a hello world message to adress 0x0F. When it recieves an
// acknowledgement, it will print this to the serial port and the amount of
// tries it took. if it fails it will print this aswell.

#include "wiwire.hpp"

int main() {
  namespace target = hwlib::target;

  auto tx = target::pin_out(target::pins::d50);
  auto rx = target::pin_in(target::pins::d51);
  auto button = target::pin_in(target::pins::d40);
  // auto led = target::pin_out( target::pins::d47 );
  // auto scl = target::pin_oc( target::pins::scl );
  // auto sda = target::pin_oc( target::pins::sda );

  // auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl,sda );
  // auto display = hwlib::glcd_oled( i2c_bus, 0x3c );
  // display.clear();

  tx.write(0);
  tx.flush();
  wiwire wire(tx, rx, 0x0B);
  int sizeMsg = 12;
  const char msg[12] = "Hello world";
  char hwtarget = 0x0F;

  while (true) {
    int tmp = wire.send(msg, sizeMsg, hwtarget);
    if (tmp == -1) {
      hwlib::cout << "failed to verify message recieved"
                  << "\n";
    } else {
      hwlib::cout << "succesfully send in " << tmp << " tries \n";
    }
    hwlib::wait_ms(1000);
    hwlib::cout << "Sending..." << '\n';
  }
  return 0;
}