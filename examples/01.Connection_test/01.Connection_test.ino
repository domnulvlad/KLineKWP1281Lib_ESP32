/*
  Title:
    01.Connection_test.ino

  Description:
    Demonstrates how to establish a connection with a module and read its identification.

  Notes:
    *The connection will be maintained while the sketch is running.
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
#ifdef module_baud_rate
  // If a specific baud rate was defined, connect with it.
  printf("Connecting with baud rate %lu\n", (unsigned long)module_baud_rate);
  diag.connect(connect_to_module, module_baud_rate);
#else
  // Otherwise, try each baud rate from the possible_baud_rates array.
  size_t current_index = 0;
  while (true)
  {
    // If connecting fails, try the next baud rate.
    printf("Trying to connect with baud rate %lu\n", possible_baud_rates[current_index]);
    if (diag.attemptConnect(connect_to_module, possible_baud_rates[current_index]) != KLineKWP1281Lib::SUCCESS)
    {
      // If there are more baud rates to try, increment the index.
      if (current_index < (sizeof(possible_baud_rates) / sizeof(possible_baud_rates[0])) - 1)
      {
        current_index++;
      }
      // Otherwise, start over.
      else
      {
        current_index = 0;
      }
      
      // Delay 1 second before trying the next baud rate.
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    // If the connection is successful, continue.
    else
    {
      break;
    }
  }
#endif

  // Display the module's part number and identification.
  printf("\n%s\n%s\n", diag.getPartNumber(), diag.getIdentification());

  // If it is available, display the module's extra identification.
  if (strlen(diag.getExtraIdentification()))
  {
    printf("%s\n", diag.getExtraIdentification());
  }
  
  // Display the module's coding value and workshop code.
  printf("\nCoding: %05u\nWSC: %05lu\n\n", diag.getCoding(), diag.getWorkshopCode());
  
  // A task must either have an infinite loop, or delete itself.
  printf("Maintaining the connection active.\n");
  while (true)
  {
    // Maintain the connection.
    diag.update();
  }
}
