/*
  Title:
    05.Single_measurement_test.ino

  Description:
    Demonstrates how to read a single measurement continuously.

  Notes:
    *You can change which group and which measurement index to read by modifying the #defines below.
    *This measurement will be read continuously while the sketch is running.
    *It is not necessary to maintain the connection with "diag.update();" in the loop, because any request will have the same effect (update() must
    be used in periods of inactivity).
*/

// Change which measurement to read, from which group.
#define GROUP_TO_READ 1 // valid range: 0-255
#define INDEX_TO_READ 0 // valid range: 0-3

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

// Please find more information on the topic of KWP1281 measurement groups in the "03.Full_measurement_test" example.
uint8_t measurement_buffer[80];
uint8_t measurement_body_buffer[4];

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
  printf("Requesting group %d, measurement index %d continuously.\n", GROUP_TO_READ, INDEX_TO_READ);
  while (true)
  {
    // Show the requested parameter in the requested group.
    showSingleMeasurement(GROUP_TO_READ, INDEX_TO_READ);
  }
}

/*

  -The variables used for "header+body" groups are stored globally.
  -Normally, the flag `received_group_header` should be reset to false when requesting a new group.
  -This example only requests one group, so it isn't necessary; if the group was "header+body" once, it will always be like that.
  
  -You could also keep `received_group_header` local to the showSingleMeasurement() function, and do diag.update() before each request,
  to ensure you receive the [Header].
  -Of course, this would negatively impact performance, since you need to receive more data from the module each time you read the group.

*/

// This flag keeps track if a [Header] was received for the current group, meaning it's of the "header+body" type.
bool received_group_header = false;

// This will contain the amount of measurements received in the [Header] of a "header+body" group.
uint8_t amount_of_measurements_in_header = 0;

void showSingleMeasurement(uint8_t group, uint8_t measurement_index)
{
  // This will contain the amount of measurements in the current group, after calling the readGroup() function.
  uint8_t amount_of_measurements = 0;
  
  /*
    -For modules which report measuring groups in the "header+body" mode, it is important to do update() when requesting a new group,
    so the module sends the [Header].
    -If it's the first request, it's not necessary; the module will always send the [Header] for the first time.
    -Since this example only requests one group, it would decrease performance (measurement rate) to do update() every time, since it would cause the module
    to send the [Header] over and over again, and it takes time to receive it, instead of only receiving it once, at the beginning.
  */
  // diag.update();
  
  /*
    The readGroup() function can return:
      KLineKWP1281Lib::FAIL                - the requested group does not exist
      KLineKWP1281Lib::ERROR               - communication error
      KLineKWP1281Lib::SUCCESS             - received measurements
      KLineKWP1281Lib::GROUP_BASIC_SETTING - received a [Basic settings] measurement; the buffer contains 10 raw values
      KLineKWP1281Lib::GROUP_HEADER        - received the [Header] for a "header+body" group; need to read again to get the [Body]
      KLineKWP1281Lib::GROUP_BODY          - received the [Body] for a "header+body" group
  */
  
  // Read the requested group and store the returned value.
  KLineKWP1281Lib::executionStatus readGroup_status;
  // If the group is not of "header+body" type, or if it is and this is the first request, we don't have a [Header] (yet), so `received_group_header=false`.
  // The response to this request will be stored in the larger array.
  // If it is in fact of "header+body" type, the [Header] will be stored in this array.
  if (!received_group_header)
  {
    readGroup_status = diag.readGroup(amount_of_measurements, group, measurement_buffer, sizeof(measurement_buffer));
  }
  // If the group is of "header+body" type, and this is not the first request, it means we have a header, so `received_group_header=true`.
  // The response to this request will be stored in the smaller array, because it should be the [Body].
  else
  {
    readGroup_status = diag.readGroup(amount_of_measurements, group, measurement_body_buffer, sizeof(measurement_body_buffer));
  }
  
  // Check the returned value.
  switch (readGroup_status)
  {
    case KLineKWP1281Lib::ERROR:
      {
        printf("Error reading group!\n");
      }
      // There is no reason to continue, exit the function.
      return;
    
    case KLineKWP1281Lib::FAIL:
      {
        printf("Group %d does not exist!\n", group);
      }
      // There is no reason to continue, exit the function.
      return;

    case KLineKWP1281Lib::GROUP_BASIC_SETTINGS:
      {
        // If we have a [Header], it means this group sends responses of "header+body" type.
        // So, at this point, it doesn't make sense to receive something other than a [Body].
        if (received_group_header)
        {
          printf("Error reading body! (got basic settings)\n");
          return;
        }

        // We have 10 raw values in the `measurement_buffer` array.
        printf("Basic settings in group %d: ", group);
        for (uint8_t i = 0; i < 10; i++)
        {
          printf("%d ", measurement_buffer[i]);
        }
        printf("\n");
      }
      // We have nothing else to display, exit the function.
      return;

    case KLineKWP1281Lib::GROUP_HEADER:
      {
        // If we have a [Header], it means this group sends responses of "header+body" type.
        // So, at this point, it doesn't make sense to receive something other than a [Body].
        if (received_group_header)
        {
          printf("Error reading body! (got header)\n");
          return;
        }

        // Set the flag to indicate that a header was received, making this a "header+body" group response.
        received_group_header = true;

        // Store the number of measurements received in the header.
        amount_of_measurements_in_header = amount_of_measurements;
      }
      // We have nothing to display yet, the next readGroup() will get the actual data; exit the function.
      return;

    case KLineKWP1281Lib::GROUP_BODY:
      {
        // If we don't have a [Header], it doesn't make sense to receive a [Body].
        if (!received_group_header)
        {
          printf("Error reading header! (got body)\n");
          return;
        }
      }
      // If we have the [Header], now we also have the [Body]; execute the code after the switch().
      break;
    
    // Execute the code after the switch().
    case KLineKWP1281Lib::SUCCESS:
      break;
  }
  
  /*
    The getMeasurementType() function can return:
      *KLineKWP1281Lib::UNKNOWN - index out of range (measurement doesn't exist in group)
      *KLineKWP1281Lib::VALUE   - regular measurement, with a value and units
      *KLineKWP1281Lib::TEXT    - text measurement
  */
  
  // Get the selected measurement's type.
  KLineKWP1281Lib::measurementType measurement_type;
  if (!received_group_header)
  {
    measurement_type = KLineKWP1281Lib::getMeasurementType(measurement_index, amount_of_measurements, measurement_buffer, sizeof(measurement_buffer));
  }
  // For "header+body" measurements, you need to use this other function that specifically parses headers instead of regular responses.
  else
  {
    measurement_type = KLineKWP1281Lib::getMeasurementTypeFromHeader(measurement_index, amount_of_measurements_in_header, measurement_buffer, sizeof(measurement_buffer));
  }
  
  // Check the returned value.
  switch (measurement_type)
  {
    // "Value and units" type
    case KLineKWP1281Lib::VALUE:
    {
      // The measurement's units will be copied into this array, so they can be displayed.
      char units_string[16];
        
      // Regular mode:
      if (!received_group_header)
      {
        // Determine how many decimal places are best suited to this measurement.
        uint8_t decimals = KLineKWP1281Lib::getMeasurementDecimals(measurement_index, amount_of_measurements, measurement_buffer, sizeof(measurement_buffer));
        
        // Display the calculated value, with the recommended amount of decimals.
        printf("%.*lf %s\n",
               decimals,
               KLineKWP1281Lib::getMeasurementValue(measurement_index, amount_of_measurements, measurement_buffer, sizeof(measurement_buffer)),
               KLineKWP1281Lib::getMeasurementUnits(measurement_index, amount_of_measurements, measurement_buffer, sizeof(measurement_buffer), units_string, sizeof(units_string)));
      }
      // "Header+body" mode:
      else
      {
        // Determine how many decimal places are best suited to this measurement.
        uint8_t decimals = KLineKWP1281Lib::getMeasurementDecimalsFromHeader(measurement_index, amount_of_measurements_in_header, measurement_buffer, sizeof(measurement_buffer));
        
        // Display the calculated value, with the recommended amount of decimals.
        printf("%.*lf %s\n",
               decimals,
               KLineKWP1281Lib::getMeasurementValueFromHeaderBody(measurement_index, amount_of_measurements_in_header, measurement_buffer, sizeof(measurement_buffer), amount_of_measurements, measurement_body_buffer, sizeof(measurement_body_buffer)),
               KLineKWP1281Lib::getMeasurementUnitsFromHeaderBody(measurement_index, amount_of_measurements_in_header, measurement_buffer, sizeof(measurement_buffer), amount_of_measurements, measurement_body_buffer, sizeof(measurement_body_buffer), units_string, sizeof(units_string)));
      }
    }
    break;
    
    // "Text" type
    case KLineKWP1281Lib::TEXT:
    {
      // The measurement's text will be copied into this array, so it can be displayed.
      char text_string[16];
      
      // Display the text.
      if (!received_group_header)
      {
        // The function getMeasurementText() returns the same string that it's given. It's the same as text_string.
        printf("%s\n", KLineKWP1281Lib::getMeasurementText(measurement_index, amount_of_measurements, measurement_buffer, sizeof(measurement_buffer), text_string, sizeof(text_string)));
      }
      else
      {
        // The function getMeasurementTextFromHeaderBody() returns the same string that it's given. It's the same as text_string.
        printf("%s\n", KLineKWP1281Lib::getMeasurementTextFromHeaderBody(measurement_index, amount_of_measurements_in_header, measurement_buffer, sizeof(measurement_buffer), amount_of_measurements, measurement_body_buffer, sizeof(measurement_body_buffer), text_string, sizeof(text_string)));
      }
    }
    break;
    
    // Invalid measurement index
    case KLineKWP1281Lib::UNKNOWN:
      printf("N/A\n");
      break;
  }
}
