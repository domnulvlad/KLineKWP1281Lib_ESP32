/*
  Title:
    06.Output_tests

  Description:
    Demonstrates how to perform output tests.

  Notes:
    *The connection will be stopped after the output tests are performed.
    *Output test descriptions are the same as fault code descriptions
      *Having KWP1281_FAULT_CODE_DESCRIPTION_SUPPORTED disabled means the descriptions will be replaced by "EN_dsc".
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

// How many milliseconds to idle on each output test
#define WAIT_TIME_MS 5000

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
  
  // Perform all output tests (procedure moved to a function).
  printf("Performing output tests.\n");
  performOutputTests();

  // Disconnect from the module.
  diag.disconnect(false);
  printf("Disconnected.\n");
  
  // A task must either have an infinite loop, or delete itself.
  vTaskDelete(NULL);
}

void performOutputTests()
{
  // Output tests are always performed in a predefined sequence that cannot be changed.
  // It is possible to determine which output test is currently running, as they return a value.
  // This value is technically a fault code, so its description can be retrieved.
  uint16_t current_output_test;
  
  /*
    The outputTests() function can return:
      *KLineKWP1281Lib::SUCCESS - performed next output test
      *KLineKWP1281Lib::FAIL    - the module doesn't support output tests / reached end of output tests
      *KLineKWP1281Lib::ERROR   - communication error
  */
  
  // Run the first output test and store the return value.
  KLineKWP1281Lib::executionStatus outputTests_status = diag.outputTests(current_output_test);
  
  // Continue running output tests until something other than SUCCESS is returned.
  while (outputTests_status == KLineKWP1281Lib::SUCCESS)
  {
    // Declare a character array and use it to store the description string.
    char description_string[32];
    KLineKWP1281Lib::getOutputTestDescription(current_output_test, description_string, sizeof(description_string));
    
    // Display the output test description.
    printf("Performing output test: %s", description_string);
    
    // Get the full length of the description string, to warn the user if the provided buffer wasn't large enough to store the entire string.
    size_t description_string_length = KLineKWP1281Lib::getOutputTestDescriptionLength(current_output_test);
    
    // If the buffer was too small, display an ellipsis and indicate how many characters would have been needed for the entire string.
    if (description_string_length > (sizeof(description_string) - 1))
    {
      printf("... (%d/%d)", sizeof(description_string) - 1, description_string_length);
    }
    printf("\n");
    
    // Wait as many milliseconds as specified by WAIT_TIME_MS.
    // The communication is kept alive by update() while waiting.
    unsigned long start_time = millis();
    while (millis() - start_time <= WAIT_TIME_MS)
    {
      diag.update();
    }
    
    // Run the next output test.
    outputTests_status = diag.outputTests(current_output_test);
  }
  
  // At this point, the loop above was stopped because either FAIL or ERROR was returned.
  
  // If FAIL was returned, the output test sequence is complete.
  if (outputTests_status == KLineKWP1281Lib::FAIL)
  {
    printf("End of output tests\n");
  }
  // If ERROR was returned, a communication error probably occurred.
  else
  {
    printf("Error performing output tests\n");
  }
}
