# WiWire
Wiwire, a simple and robust wireless transmission protocol for embedded systems

# Warning: This project is made as a education excercise, don't use it in real world applications

## Made by Otto de Visser
first year student at Hogeschool Utrecht

## Uses
This project uses https://github.com/wovo/hwlib to send and recieve data point to point. This project is made to be used with very basic 433MHz senders and recievers, but there is no inherent problem that limits its use to this only. 
the basic idea was to make a more robust replacement for a protocol like I2C, with the capability to verify send message, request resends and confirm send messages have been recieved in good health.

## How to use
The usage of this libary is rather easy. Make a pin_in object and a pin_out object, one for recieving and one for sending and feed these to the constructor of the WiWire class. You can now send, broadcast and recieve messages. 

# Example uses
One might use this in a IOT environment, where one can broadcast tasks to all avalable devices or to talk to a indivitual device using its hardware id. Think along the lines of turing on individual light, with dimming and color info, and using a broadcast to quickly turn them all off. 

