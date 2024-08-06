// Select your target module addresses:
uint8_t connect_to_modules[] = {0x01, 0x17}; // K-Line 1, K-Line 2

// Select your target module speed:
unsigned long module_baud_rates[] = {10400, 9600}; // K-Line 1, K-Line 2

// Enable/disable printing bus traffic on the Serial Monitor.
#define debug_traffic false

// Uncomment one of the following options, and change the pins to your requirements:

// ESP32 (can use Serial1 and Serial2)
/*
  #define K_line1 Serial1
  #define TX1_pin 17
  #define RX1_pin 16
  
  #define K_line2 Serial2
  #define TX2_pin 27
  #define RX2_pin 26
*/

#if not defined(K_line1) or not defined(K_line2)
  #error Please select an option in configuration.h!
#endif
