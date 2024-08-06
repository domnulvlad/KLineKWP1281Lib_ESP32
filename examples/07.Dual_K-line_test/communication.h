//////////////////////////////// K-line 1

// This handle refers to the task managing K-Line 1, in order to be able to send a notification to it.
TaskHandle_t demo_task1_handle;

// Callback executed by the serial event task when serial data is available on K-Line 1
void onReceiveFunction1()
{
  // Send a notification to the task managing K-Line 1.
  xTaskNotify(demo_task1_handle, 0, eNoAction);
}

// Initialize the serial port
void beginFunction1(unsigned long baud) {
  // Start serial communication.
  K_line1.begin(baud, SERIAL_8N1, RX1_pin, TX1_pin);
  
  // Attach the callback that executes every time a byte is received.
  K_line1.onReceive(onReceiveFunction1, false);
  K_line1.setRxFIFOFull(1);

  // Reset the task's notification state.
  xTaskNotifyStateClear(NULL);
}

// Stop communication on the serial port
void endFunction1() {
  K_line1.end();
}

// Send a byte
void sendFunction1(uint8_t data) {
  K_line1.write(data);
}

// Receive a byte
bool receiveFunction1(uint8_t *data, unsigned long timeout_ticks) {
  // If no byte is immediately available, wait.
  if (K_line1.available() <= 0)
  {
    // Wait until either a notification is received, or a timeout occurs.
    if (xTaskNotifyWait(0, 0, NULL, timeout_ticks) != pdTRUE)
    {
      return false;
    }
  }

  // A byte is available; store it in the variable passed by pointer.
  *data = K_line1.read();
  return true;
}

// Debugging can be enabled in configuration.h in order to print bus traffic on the Serial Monitor.
#if debug_traffic
void KWP1281debugFunction1(bool direction, uint8_t message_sequence, uint8_t message_type, uint8_t *data, size_t length)
{
  printf("\t1.%s S:%02X T:%02X L:%d ", (direction ? "RECEIVE" : "SEND"), message_sequence, message_type, length);

  if (length)
  {
    printf("D:");

    // Iterate through the bytes of the message.
    for (size_t i = 0; i < length; i++)
    {
      printf("%02X ", data[i]);
    }
  }

  printf("\n");
}
#endif

//////////////////////////////// K-line 2

// This handle refers to the task managing K-Line 2, in order to be able to send a notification to it.
TaskHandle_t demo_task2_handle;

// Callback executed by the serial event task when serial data is available on K-Line 2
void onReceiveFunction2()
{
  // Send a notification to the task managing K-Line 2.
  xTaskNotify(demo_task2_handle, 0, eNoAction);
}

// Initialize the serial port
void beginFunction2(unsigned long baud) {
  // Start serial communication.
  K_line2.begin(baud, SERIAL_8N1, RX2_pin, TX2_pin);
  
  // Attach the callback that executes every time a byte is received.
  K_line2.onReceive(onReceiveFunction2, false);
  K_line2.setRxFIFOFull(1);

  // Reset the task's notification state.
  xTaskNotifyStateClear(NULL);
}

// Stop communication on the serial port
void endFunction2() {
  K_line2.end();
}

// Send a byte
void sendFunction2(uint8_t data) {
  K_line2.write(data);
}

// Receive a byte
bool receiveFunction2(uint8_t *data, unsigned long timeout_ticks) {
  // If no byte is immediately available, wait.
  if (K_line2.available() <= 0)
  {
    // Wait until either a notification is received, or a timeout occurs.
    if (xTaskNotifyWait(0, 0, NULL, timeout_ticks) != pdTRUE)
    {
      return false;
    }
  }

  // A byte is available; store it in the variable passed by pointer.
  *data = K_line2.read();
  return true;
}

// Debugging can be enabled in configuration.h in order to print bus traffic on the Serial Monitor.
#if debug_traffic
void KWP1281debugFunction2(bool direction, uint8_t message_sequence, uint8_t message_type, uint8_t *data, size_t length)
{
  printf("\t2.%s S:%02X T:%02X L:%d ", (direction ? "RECEIVE" : "SEND"), message_sequence, message_type, length);

  if (length)
  {
    printf("D:");

    // Iterate through the bytes of the message.
    for (size_t i = 0; i < length; i++)
    {
      printf("%02X ", data[i]);
    }
  }

  printf("\n");
}
#endif
