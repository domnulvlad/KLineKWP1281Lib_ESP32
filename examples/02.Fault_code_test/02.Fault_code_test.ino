/*
  Title:
    02.Fault_code_test.ino

  Description:
    Demonstrates how to read a module's fault codes and their elaborations.

  Notes:
    *The connection will be stopped after the fault codes are read.
    *If you have fault code descriptions enabled (KWP1281_FAULT_CODE_DESCRIPTION_SUPPORTED/KWP1281_OBD_FAULT_CODE_DESCRIPTION_SUPPORTED),
    you can choose from a few different languages a bit further below in "KLineKWP1281Lib.h".
      *Please only choose one option.
      *If they are disabled, the descriptions will be replaced by "EN_dsc"/"EN_obd".
    *If you have fault code elaborations enabled (KWP1281_FAULT_CODE_ELABORATION_SUPPORTED), you can choose from a few different languages
    a bit further below in "KLineKWP1281Lib.h".
      *Please only choose one option.
      *If they are disabled, the elaborations will be replaced by "EN_elb".
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

// Each fault code takes up 3 bytes.
// You can increase the value below if you get "Too many faults for the given buffer size" during the test.
#define DTC_BUFFER_MAX_FAULT_CODES 16
uint8_t faults[3 * DTC_BUFFER_MAX_FAULT_CODES];

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
  /*
    *There is an optional argument for connect()/attemptConnect(), which, if set to false, will make it so extra identification will not be requested
    if the module supports it.
    *This would save almost one second of the connection time, with the downside being that getExtraIdentification() will return an empty string.
    *It has no effect if the module doesn't support extra identification.
  */
  diag.connect(connect_to_module, module_baud_rate, false);
  
  // Show the module's fault codes (procedure moved to a function).
  printf("Requesting fault codes.\n");
  showDTCs();

  // Disconnect from the module.
  /*
    *There is an optional argument for disconnect(), which, if set to false, will make it so the library doesn't wait for a response.
    *When disconnecting, some modules send a response, some don't.
    *If the module doesn't send a response, not waiting saves [diag.responseTimeout] milliseconds, by default 2 seconds.
    *If the module sends a response, not waiting for it shouldn't really have consequences.
  */
  diag.disconnect(false);
  printf("Disconnected.\n");
  
  // A task must either have an infinite loop, or delete itself.
  vTaskDelete(NULL);
}

void showDTCs()
{
  // This will contain the amount of fault codes, after calling the readFaults() function.
  uint8_t amount_of_fault_codes = 0;
  
  /*
    Always check the return value of functions!
    
    The readFaults() function can return:
      *KLineKWP1281Lib::SUCCESS - received fault codes
      *KLineKWP1281Lib::FAIL    - the module doesn't support fault codes
      *KLineKWP1281Lib::ERROR   - communication error
  */

  // If fault codes were read successfully, display them.
  if (diag.readFaults(amount_of_fault_codes, faults, sizeof(faults)) == KLineKWP1281Lib::SUCCESS)
  {
    // Display how many fault codes were received.
    printf("DTCs: %d\n", amount_of_fault_codes);

    // Each fault code takes up 3 bytes.
    // Check if the buffer was large enough for all fault codes.
    // If it wasn't, only attempt to display as many as fit.
    uint8_t available_fault_codes = amount_of_fault_codes;
    if (sizeof(faults) < amount_of_fault_codes * 3)
    {
      printf("Too many faults for the given buffer size");
      available_fault_codes = sizeof(faults) / 3;
    }

    // Print each fault code.
    for (uint8_t i = 0; i < available_fault_codes; i++)
    {
      // Retrieve the "i"-th fault code and the elaboration code from the buffer.
      uint16_t fault_code = KLineKWP1281Lib::getFaultCode(i, available_fault_codes, faults, sizeof(faults));
      uint8_t fault_elaboration_code = KLineKWP1281Lib::getFaultElaborationCode(i, available_fault_codes, faults, sizeof(faults));
      
      // Declare a character array and use it to store the description string of the fault code.
      char description_string[32];
      KLineKWP1281Lib::getFaultDescription(fault_code, description_string, sizeof(description_string));
      
      // Print the fault code, description string.
      printf("  %05u - %s", fault_code, description_string);

      // Get the full length of the description string, to warn the user if the provided buffer wasn't large enough to store the entire string.
      size_t description_string_length = KLineKWP1281Lib::getFaultDescriptionLength(fault_code);

      // If the buffer was too small, display an ellipsis and indicate how many characters would have been needed for the entire string.
      if (description_string_length > (sizeof(description_string) - 1))
      {
        printf("... (%d/%d)", sizeof(description_string) - 1, description_string_length);
      }
      printf("\n");
      
      // Store the elaboration code of the fault (without the high bit) in a string.
      char fault_elaboration_code_string[8];
      sprintf(fault_elaboration_code_string, "%02u-%02u", fault_elaboration_code & ~0x80, ((fault_elaboration_code & 0x80) ? 10 : 0));
      
      // This will indicate whether or not the fault is intermittent, after calling the getFaultElaboration() function.
      bool is_intermittent;

      // Declare a character array and use it to store the elaboration string.
      char elaboration_string[32];
      KLineKWP1281Lib::getFaultElaboration(is_intermittent, fault_elaboration_code, elaboration_string, sizeof(elaboration_string));
      
      // A fault code may or may not be of "OBD" type.
      // OBD fault codes should be displayed formatted according to the standard, for example "P0001" instead of "16385".
      if (KLineKWP1281Lib::isOBDFaultCode(fault_code))
      {
        // Declare a character array and use it to store the formatted fault code.
        char OBD_fault_code_string[6];
        KLineKWP1281Lib::getOBDFaultCode(fault_code, OBD_fault_code_string, sizeof(OBD_fault_code_string));
        
        // Print the formatted fault code and elaboration code.
        printf("    %s - %s", OBD_fault_code_string, fault_elaboration_code_string);
        
        // The elaboration produced by getFaultElaboration() may not be accurate for OBD fault codes.
        // The proper elaboration is contained in the description string, after the last ':' character.
      }
      else
      {
        // Print the fault elaboration code and elaboration text.
        printf("    %s - %s", fault_elaboration_code_string, elaboration_string);
        
        // Get the full length of the elaboration string, to warn the user if the provided buffer wasn't large enough to store the entire string.
        size_t elaboration_string_length = KLineKWP1281Lib::getFaultElaborationLength(i, available_fault_codes, faults, sizeof(faults));
  
        // If the buffer was too small, display an ellipsis and indicate how many characters would have been needed for the entire string.
        if (elaboration_string_length > (sizeof(elaboration_string) - 1))
        {
          printf("... (%d/%d)", sizeof(elaboration_string) - 1, elaboration_string_length);
        }
      }

      // If the fault is intermittent, display this info.
      if (is_intermittent)
      {
        printf(" - Intermittent");
      }
      printf("\n");
    }
  }
  // If fault codes were not read successfully, show an error.
  else
  {
    printf("Error reading DTCs\n");
  }
}
