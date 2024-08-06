/*
  Title:
    03.Full_measurement_test.ino

  Description:
    Demonstrates how to read a module's measuring blocks.

  Notes:
    *Measuring blocks 0-255 will be read, after which the connection will be stopped.
    *If you have the text table enabled (KWP1281_TEXT_TABLE_SUPPORTED), you can choose from a few different languages a bit further below
    in "KLineKWP1281Lib.h".
      *Please only choose one option.
      *If it is disabled, the value of some parameters will be replaced by "EN_f25".
*/

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
  
  // Read all groups (procedure moved to a function).
  printf("Requesting measuring blocks 000-255.\n");
  for (uint16_t i = 0; i <= 255; i++)
  {
    showMeasurements(i);
  }
  
  // Disconnect from the module.
  diag.disconnect(false);
  printf("Disconnected.\n");
  
  // A task must either have an infinite loop, or delete itself.
  vTaskDelete(NULL);
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
    
    // You can retrieve the "formula" byte for a measurement, to avoid giving all these parameters to the other functions.
    uint8_t formula = KLineKWP1281Lib::getFormula(i, amount_of_measurements, measurements, sizeof(measurements));
    
    /*
      The getMeasurementType() function can return:
        *KLineKWP1281Lib::UNKNOWN - index out of range (measurement doesn't exist in block)
        *KLineKWP1281Lib::VALUE   - regular measurement, with a value and units
        *KLineKWP1281Lib::TEXT    - text measurement
    */
    
    // Get the current measurement's type.
    KLineKWP1281Lib::measurementType measurement_type = KLineKWP1281Lib::getMeasurementType(formula);
    // If you don't want to extract the "formula" byte as shown above with the getFormula() function, getMeasurementType() can also take the same parameters
    // like the other functions (index, amount, buffer, buffer_size).
    
    // Check the return value.
    switch (measurement_type)
    {
      // "Value and units" type
      case KLineKWP1281Lib::VALUE:
      {
        // You can retrieve the other significant bytes for a measurement, to avoid giving all these parameters to the other functions.
        uint8_t NWb = KLineKWP1281Lib::getNWb(i, amount_of_measurements, measurements, sizeof(measurements));
        uint8_t MWb = KLineKWP1281Lib::getMWb(i, amount_of_measurements, measurements, sizeof(measurements));
        
        // This will hold the measurement's units.
        char units_string[16];
        
        // Get the current measurement's value.
        double value = KLineKWP1281Lib::getMeasurementValue(formula, NWb, MWb);
        // If you don't want to extract the "formula", "NWb" and "MWb" bytes as shown above with the getFormula(), getNWb() and getMWb() functions,
        // getMeasurementValue() and getMeasurementUnits() can also take the same parameters like the other functions (index, amount, buffer, buffer_size).
        
        // Get the current measurement's units.
        KLineKWP1281Lib::getMeasurementUnits(formula, NWb, MWb, units_string, sizeof(units_string));
        // The getMeasurementUnits() function returns the same string it's given, units_string in this case.
        
        // Determine how many decimal places are best suited to this measurement.
        uint8_t decimals = KLineKWP1281Lib::getMeasurementDecimals(formula);
        // getMeasurementDecimals() only needs to know the "formula", but you can also give it all parameters as with all other functions.
        
        //Display the calculated value, with the recommended amount of decimals.
        printf("%.*lf %s\n", decimals, value, units_string);
      }
      break;
      
      // "Text" type
      case KLineKWP1281Lib::TEXT:
      {
        // This will hold the measurement's text.
        char text_string[16];
        
        // The only important values are stored in the text string.
        // The getMeasurementUnits() function needs more data than just those 3 bytes.
        // An example of extracting this data is given in the "Dual_K-line_test" demo.
        // It's easier to just give it all parameters, like done here.
        KLineKWP1281Lib::getMeasurementText(i, amount_of_measurements, measurements, sizeof(measurements), text_string, sizeof(text_string));
        
        // Display the text.
        printf("%s\n", text_string);
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
