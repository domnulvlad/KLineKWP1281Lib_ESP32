/*
  Title:
    07.Dual_K-Line_test.ino

  Description:
    Demonstrates how to connect to 2 K-Lines at once and read measurements.

  Notes:
    *This demo is made specifically for the ESP32, as native multitasking is needed. Also, it has 2 hardware serial ports.
    *It doesn't make sense to have 2 transceivers on the same K-Line! This is only useful if you decide to separate a specific module from the
    car's K-Line in order to be able to access it asyncronously.

    *You can change which measurements to read by modifying the arrays below.
    *These measurements will be read continuously while the sketch is running.
    *It is not necessary to maintain the connection with "diag.update();" in the loop, because any request will have the same effect (update() must
    be used in periods of inactivity).
*/

/*
  ***Uncomment the appropriate options in the "configuration.h" file!

  ESP32
     has two additional serial ports
     pins (they can be remapped, this is what they are configured to in this example):
       K-Line 1 TX -> RX pin 16
       K-Line 1 RX -> TX pin 17
       K-Line 2 TX -> RX pin 26
       K-Line 2 RX -> TX pin 27
*/

// Change which measurement to read from each K-Line.
uint8_t blocks_to_read[] = {1, 1}; // K-Line 1, K-Line 2; valid range: 0-255
uint8_t indexes_to_read[] = {0, 0}; // K-Line 1, K-Line 2; valid range: 0-3

// Include the library.
#include <KLineKWP1281Lib_ESP32.h>

// Include the two files containing configuration options and the functions used for communication.
#include "configuration.h"
#include "communication.h"

// Debugging can be enabled in the "Tools>Core Debug Level" menu in the IDE, in order to print connection-related info on the Serial Monitor.
KLineKWP1281Lib diag1(beginFunction1, endFunction1, sendFunction1, receiveFunction1, TX1_pin);
KLineKWP1281Lib diag2(beginFunction2, endFunction2, sendFunction2, receiveFunction2, TX2_pin);

// Put pointers to both instances in an array, to be able to use them by indexing into the array.
KLineKWP1281Lib *diags[] = {&diag1, &diag2};

// Normally, each measurement takes 3 bytes, and a block can store up to 4 measurements.
#define MEASUREMENT_BUFFER_SIZE 3 * 4

// Structure to hold all data necessary for processing a measurement and identifying which K-Line the data was read from
struct kline_measurement_structure
{
  uint8_t K_line_index;
  uint8_t formula;
  uint8_t NWb;
  uint8_t MWb;
  uint8_t measurement_data[MEASUREMENT_BUFFER_SIZE];
  uint8_t measurement_data_length;
};

// Queue which will contain structures like the one defined above
// Through it, both K-Line tasks will send their data to the main processing task.
QueueHandle_t kline_measurement_queue;

void setup()
{
  // Initialize the Serial Monitor.
  Serial.begin(115200);
  delay(500);
  printf("Sketch started.\n");

  // If debugging bus traffic was enabled, attach the debugging functions.
#if debug_traffic
  diag1.KWP1281debugFunction(KWP1281debugFunction1);
  diag2.KWP1281debugFunction(KWP1281debugFunction2);
#endif
  
  // Create the queue, able to hold 10 measurement structures.
  assert(kline_measurement_queue = xQueueCreate(10, sizeof(kline_measurement_structure)));
  
  // Create the task which will display the data coming from the two modules.
  // It has a higher priority than the K-Line tasks, because it mostly waits for data.
  // Having a higher priority means it will be able to wake up and process the data right when it arrives.
  xTaskCreate(receiver_task, "recv", 4096, NULL, 2, NULL);

  printf("Requesting block %d, measurement %d from K-Line 1 and block %d, measurement %d from K-Line 2 continuously.\n",
         blocks_to_read[0], indexes_to_read[0],
         blocks_to_read[1], indexes_to_read[1]);

  // Create a task for each K-Line.
  // The two handles defined in "configuration.h" are attached to the tasks, so the Serial onReceive functions
  // can send notifications to them.
  xTaskCreate(kline_task, "K1", 4096, (void*)0, 1, &demo_task1_handle);
  xTaskCreate(kline_task, "K2", 4096, (void*)1, 1, &demo_task2_handle);
  // Both tasks share the same code, but they are differentiated through the argument passed to the function,
  // which indicates the index to be used for accessing the arrays defined at the top of the sketch.
}

void loop()
{
  // The Arduino loop is not used; delete its task.
  vTaskDelete(NULL);
}

void receiver_task(void *args)
{
  // This structure will hold data, either from K-Line 1 or K-Line 2.
  kline_measurement_structure measurement_structure;
  
  // A task must have an infinite loop.
  while (true)
  {
    // Wait for data to be available on the queue.
    // The timeout of portMAX_DELAY means this will wait however necessary (possibly indefinitely) for data to be available.
    xQueueReceive(kline_measurement_queue, &measurement_structure, portMAX_DELAY);
    
    // Display the data.
    displayMeasurement(&measurement_structure);
    
    // The loop will repeat, waiting again.
  }
}

void kline_task(void *args)
{
  // This function is shared by both tasks; they are differentiated through the number stored in the "args" variable.
  
  // Retrieve the K-Line index (0/1), which was passed through the parameter.
  uint8_t K_line_index = (uint32_t)args;
  
  // Retrieve a pointer to the correct diag instance.
  KLineKWP1281Lib *diag = diags[K_line_index];
  
  // Connect to the desired module.
  // "diag" is a pointer, so "->" is used for accessing its functions, instead of ".".
  diag->connect(connect_to_modules[K_line_index], module_baud_rates[K_line_index]);
  
  // This buffer will be used for receiving measurements.
  uint8_t measurements[MEASUREMENT_BUFFER_SIZE];

  // A task must have an infinite loop.
  while (true)
  {
    // This function needs the K-Line index, because it puts it in the struct that gets sent on the queue.
    // It is given the arrays directly, and it retrieves the objects itself using the index.
    getSingleMeasurement(K_line_index, blocks_to_read, indexes_to_read, diags, measurements, sizeof(measurements));
  }
}

/*
  K_line_index            -> which K-Line to send the request on (0/1)
  blocks                  -> array of 2 block IDs; which group to request from each K-Line
  indexes                 -> array of 2 measurement indexes; which index to retrieve from the block requested from each K-Line
  diags                   -> array of 2 pointers to diag instances
  measurement_buffer      -> array to give to readGroup()
  measurement_buffer_size -> array size to give to readGroup()
*/
void getSingleMeasurement(uint8_t K_line_index, uint8_t *blocks, uint8_t *indexes, KLineKWP1281Lib **diags, uint8_t *measurement_buffer, size_t measurement_buffer_size)
{
  // Retrieve each object from the arrays, using the index.
  uint8_t block = blocks[K_line_index];
  uint8_t index = indexes[K_line_index];
  KLineKWP1281Lib *diag = diags[K_line_index];
  
  // This structure will contain the received measurement; set its index, to indicate which K-Line the measurement was requested from.
  kline_measurement_structure measurement_structure;
  measurement_structure.K_line_index = K_line_index;
  
  // This will contain the amount of measurements in the current block, after calling the readGroup() function.
  uint8_t amount_of_measurements = 0;
  
  /*
    The readGroup() function can return:
       KLineKWP1281Lib::SUCCESS - received measurements
       KLineKWP1281Lib::FAIL    - the requested block does not exist
       KLineKWP1281Lib::ERROR   - communication error
  */
  
  // Read the requested group and store the return value.
  // "diag" is a pointer, so "->" is used for accessing its functions, instead of ".".
  KLineKWP1281Lib::executionStatus readGroup_status = diag->readGroup(amount_of_measurements, block, measurement_buffer, measurement_buffer_size);
  
  switch (readGroup_status)
  {
    case KLineKWP1281Lib::ERROR:
      printf("Error reading measurements on K%d!\n", K_line_index + 1);
      return;

    case KLineKWP1281Lib::FAIL:
      printf("Block %d does not exist on K%d!\n", block, K_line_index + 1);
      return;

    //Execute the code after the switch().
    case KLineKWP1281Lib::SUCCESS:
      break;
  }
  
  /*
    *Most measurements only need the "formula" (type), "NWb" (byte A) and "MWb" (byte B).
    *To determine the type of a measurement with the getMeasurementType() function, only the formula is needed.
    
    *For measurements of type VALUE, you need to get the value and units.
    *You can call getMeasurementValue() with those 3 parameters mentioned above, instead of all (index, amount, buffer, buffer size).
    *The same applies to getMeasurementUnits().
    
    *There are a few measurements which may contain more than 2 data bytes (for example, if the module sends long text or a field of values).
    *These measurements will always be of the TEXT type, even if most of them still only use the 2 data bytes.
    
    *So the getMeasurementText() function can either be called with all parameters, or with the data buffer and its length.
    *In this case, the data buffer and its length can be retrieved with getMeasurementData() and getMeasurementDataLength().    
    *The function getMeasurementData() returns a pointer into the measurement buffer, so its contents must be copied to avoid loss!
    
    *You cannot call getMeasurementText() with only those 3 bytes!
  */
  
  // Retrieve the 3 bytes into the structure.
  measurement_structure.formula = KLineKWP1281Lib::getFormula(index, amount_of_measurements, measurement_buffer, measurement_buffer_size);
  measurement_structure.NWb = KLineKWP1281Lib::getNWb(index, amount_of_measurements, measurement_buffer, measurement_buffer_size);
  measurement_structure.MWb = KLineKWP1281Lib::getMWb(index, amount_of_measurements, measurement_buffer, measurement_buffer_size);
  
  // Retrieve the data buffer and its length, because some measurements may need it.
  uint8_t measurement_data_length = KLineKWP1281Lib::getMeasurementDataLength(index, amount_of_measurements, measurement_buffer, measurement_buffer_size);
  uint8_t *measurement_data = KLineKWP1281Lib::getMeasurementData(index, amount_of_measurements, measurement_buffer, measurement_buffer_size);
  
  // Copy the data buffer and its length into the structure.
  measurement_structure.measurement_data_length = measurement_data_length;
  memcpy(measurement_structure.measurement_data, measurement_data, measurement_data_length);
  
  // Put the structure on the queue; don't wait for the queue to free up if it is full.
  xQueueSend(kline_measurement_queue, &measurement_structure, 0);
}

void displayMeasurement(kline_measurement_structure *measurement_structure)
{
  // Show which K-Line the measurement was requested from.
  // "measurement_structure" is a pointer, so "->" is used for accessing its members, instead of ".".
  printf("K%d: ", measurement_structure->K_line_index + 1);
  
  /*
    The getMeasurementType() function can return:
      *KLineKWP1281Lib::UNKNOWN - index out of range (measurement doesn't exist in block)
      *KLineKWP1281Lib::VALUE   - regular measurement, with a value and units
      *KLineKWP1281Lib::TEXT    - text measurement
  */
  
  // Get the measurement's type and check the return value.
  switch (KLineKWP1281Lib::getMeasurementType(measurement_structure->formula))
  {
    // "Value and units" type
    case KLineKWP1281Lib::VALUE:
    {
      // This will hold the measurement's units.
      char units_string[16];
      
      // Display the calculated value, with the recommended amount of decimals.
      printf("%.*lf %s\n",
             KLineKWP1281Lib::getMeasurementDecimals(measurement_structure->formula),
             KLineKWP1281Lib::getMeasurementValue(measurement_structure->formula, measurement_structure->NWb, measurement_structure->MWb),
             KLineKWP1281Lib::getMeasurementUnits(measurement_structure->formula, measurement_structure->NWb, measurement_structure->MWb, units_string, sizeof(units_string)));
    }
    break;

    // "Text" type
    case KLineKWP1281Lib::TEXT:
    {
      // This will hold the measurement's text.
      char text_string[16];
      
      // Display the text.      
      printf("%s\n", KLineKWP1281Lib::getMeasurementText(measurement_structure->formula, measurement_structure->measurement_data, measurement_structure->measurement_data_length, text_string, sizeof(text_string)));
    }
    break;

    // Invalid measurement index
    case KLineKWP1281Lib::UNKNOWN:
      printf("N/A\n");
      break;
  }
}
