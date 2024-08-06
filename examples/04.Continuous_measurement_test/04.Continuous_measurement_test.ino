/*
  Title:
    04.Continuous_measurement_test.ino

  Description:
    Demonstrates how to read a measurement block continuously.

  Notes:
    *You can change which block to read by modifying the #define below.
    *This block will be read continuously while the sketch is running.
    *It is not necessary to maintain the connection with "diag.update();" in the loop, because any request will have the same effect (update() must
    be used in periods of inactivity).
*/

// Change which block to read.
#define BLOCK_TO_READ 1

/*
  ***Uncomment the appropriate options in the "configuration.h" file!

  ESP32
    *has two additional serial ports
    *pins (they can be remapped, this is what they are configured to in these examples):
      *K-line TX -> RX pin 16
      *K-line RX -> TX pin 17

  ESP32-C6
    *has one additional serial port
    *pins (they can be remapped, this is what they are configured to in these examples):
      *K-line TX -> RX pin 16
      *K-line RX -> TX pin 17
*/

// Include the library.
#include <KLineKWP1281Lib_ESP32.h>

// Include the two files containing configuration options and the functions used for communication.
#include "configuration.h"
#include "communication.h"

// Debugging can be enabled in the "Tools>Core Debug Level" menu in the IDE, in order to print connection-related info on the Serial Monitor.
KLineKWP1281Lib diag(beginFunction, endFunction, sendFunction, receiveFunction, TX_pin);

// Normally, each measurement takes 3 bytes, and a block can store up to 4 measurements.
// There are some measurements which take more space.
// Consider increasing the size of this buffer if you get "Error reading measurements!":
uint8_t measurements[3 * 4];

void setup()
{
  // Initialize the Serial Monitor.
  Serial.begin(115200);
  delay(500);
  printf("Sketch started.\n");
  
  // If debugging bus traffic was enabled, attach the debugging function.
#if debug_traffic
  diag.KWP1281debugFunction(KWP1281debugFunction);
#endif
  
  // Create the task that runs this demo, attaching the handle defined in "configuration.h".
  assert(xTaskCreate(demo_task, "demo", 4096, NULL, 5, &demo_task_handle));
}

void loop()
{
  // The Arduino loop is not used; delete its task.
  vTaskDelete(NULL);
}

void demo_task(void *args)
{
  // Connect to the module.
  diag.connect(connect_to_module, module_baud_rate, false);
  
  // A task must either have an infinite loop, or delete itself.
  printf("Requesting block %d continuously.\n", BLOCK_TO_READ);
  while (true)
  {
    // Show the parameters in the requested block.
    showMeasurements(BLOCK_TO_READ);
  }
}

void showMeasurements(uint8_t block)
{
  // This will contain the amount of measurements in the current block, after calling the readGroup() function.
  uint8_t amount_of_measurements = 0;
  
  /*
    The readGroup() function can return:
      *KLineKWP1281Lib::SUCCESS - received measurements
      *KLineKWP1281Lib::FAIL    - the requested block does not exist
      *KLineKWP1281Lib::ERROR   - communication error
  */
  
  // Read the requested group and store the return value.
  KLineKWP1281Lib::executionStatus readGroup_status = diag.readGroup(amount_of_measurements, block, measurements, sizeof(measurements));
  
  // Check the return value.
  switch (readGroup_status)
  {
    case KLineKWP1281Lib::ERROR:
      printf("Error reading measurements!\n");
      return;
    
    case KLineKWP1281Lib::FAIL:
      printf("Block %d does not exist!\n", block);
      return;
    
    // Execute the code after the switch().
    case KLineKWP1281Lib::SUCCESS:
      break;
  }
  
  // If the block was read successfully, display its measurements.
  printf("Block %d:\n", block);
  
  // Display each measurement.
  for (uint8_t i = 0; i < 4; i++)
  {
    //Format the values with a leading tab.
    printf("\t");
    
    /*
      The getMeasurementType() function can return:
        *KLineKWP1281Lib::UNKNOWN - index out of range (measurement doesn't exist in block)
        *KLineKWP1281Lib::VALUE   - regular measurement, with a value and units
        *KLineKWP1281Lib::TEXT    - text measurement
    */
    
    //Get the current measurement's type and check the return value.
    switch (KLineKWP1281Lib::getMeasurementType(i, amount_of_measurements, measurements, sizeof(measurements)))
    {
      // "Value and units" type
      case KLineKWP1281Lib::VALUE:
      {
        // This will hold the measurement's units.
        char units_string[16];
        
        // Display the calculated value, with the recommended amount of decimals.
        // The function getMeasurementUnits() returns the same string that it's given. It's the same as units_string.
        printf("%.*lf %s\n",
               KLineKWP1281Lib::getMeasurementDecimals(i, amount_of_measurements, measurements, sizeof(measurements)),
               KLineKWP1281Lib::getMeasurementValue(i, amount_of_measurements, measurements, sizeof(measurements)),
               KLineKWP1281Lib::getMeasurementUnits(i, amount_of_measurements, measurements, sizeof(measurements), units_string, sizeof(units_string)));
      }
      break;
      
      // "Text" type
      case KLineKWP1281Lib::TEXT:
      {
        // This will hold the measurement's text.
        char text_string[16];
        
        // The function getMeasurementText() returns the same string that it's given. It's the same as text_string.
        printf("%s\n", KLineKWP1281Lib::getMeasurementText(i, amount_of_measurements, measurements, sizeof(measurements), text_string, sizeof(text_string)));
      }
      break;
      
      // Invalid measurement index
      case KLineKWP1281Lib::UNKNOWN:
        printf("N/A\n");
        break;
    }
  }

  // Leave an empty line.
  printf("\n");
}
