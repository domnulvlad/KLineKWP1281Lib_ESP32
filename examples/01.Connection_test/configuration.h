// Select your target module address:
#define connect_to_module 0x01

// Select your target module speed:
//#define module_baud_rate 10400

// If you don't define a baud rate above, connecting will be attempted at the following speeds:
const unsigned long possible_baud_rates[] = {10400, 9600, 4800};

// Enable/disable printing bus traffic on the Serial Monitor.
#define debug_traffic false

// Uncomment one of the following options, and change the pins to your requirements:

// ESP32 (can use Serial1/Serial2)
/*
  #define K_line Serial1
  #define TX_pin 17
  #define RX_pin 16
*/

// ESP32-C6 (can use Serial1)
/*
  #define K_line Serial1
  #define TX_pin 17
  #define RX_pin 16
*/

#ifndef K_line
  #error Please select an option in configuration.h!
#endif
