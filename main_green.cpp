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
  
  auto whiteButton = target::pin_in( target::pins::d47 );
  auto blueButton = target::pin_in( target::pins::d46 );
  auto greenButton = target::pin_in( target::pins::d44 );
  auto yellowButton = target::pin_in( target::pins::d43 );

  auto broadcastButton = target::pin_in( target::pins::d45 );

  auto whiteLed = target::pin_out(target::pins::d40);
  auto blueLed = target::pin_out(target::pins::d39);
  auto redLed = target::pin_out(target::pins::d38);
  auto greenLed = target::pin_out(target::pins::d37);
  auto yellowLed = target::pin_out(target::pins::d36);
  

  // auto scl = target::pin_oc( target::pins::scl );
  // auto sda = target::pin_oc( target::pins::sda );

  // auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl,sda );
  // auto display = hwlib::glcd_oled( i2c_bus, 0x3c );
  // display.clear();

  yellowLed.write(0); yellowLed.flush();

  tx.write(0);
  tx.flush();
  wiwire wire(tx, rx, 0x0B);
  int sizeMsg = 12;
  const char msg[12] = "Hello world";
  char hwtargetWhite = 0x0A;
  // char hwtargetBlue = 0x0B;
  // char hwtargetGreen = 0x0C;
  // char hwtargetYellow = 0x0D;

  while (true) {
    if (whiteButton.read())
    {
      wire.send(msg, sizeMsg, hwtargetWhite);
    }
    char msgRecieved[12] = {};
    int size = wire.blockRead(msgRecieved);
    bool same = true;
    for (int i = 0; i < size; i++)
    {
      if (msg[i] != msgRecieved[i])
      {
        same = false;
      }
    }
    if (same)
    {
      blueLed.write(1); blueLed.flush();
      hwlib::wait_ms(1000);
      blueLed.write(0); blueLed.flush();
    }
  }
  return 0;
}