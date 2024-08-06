// This handle refers to the task managing the K-Line, in order to be able to send a notification to it.
TaskHandle_t demo_task_handle;

// Callback executed by the serial event task when serial data is available
void onReceiveFunction()
{
  // Send a notification to the task managing the K-Line.
  xTaskNotify(demo_task_handle, 0, eNoAction);
  
  // It is also possible to use an event group, semaphore or queue for the purpose of signaling that a byte was received.
  // If you don't want to use task notifications, also change the code in beginFunction() and receiveFunction() below.
}

// Initialize the serial port
void beginFunction(unsigned long baud)
{
  // Start serial communication.
  K_line.begin(baud, SERIAL_8N1, RX_pin, TX_pin);
  
  // Attach the callback that executes every time a byte is received.
  K_line.onReceive(onReceiveFunction, false);
  K_line.setRxFIFOFull(1);

  // Reset the task's notification state.
  xTaskNotifyStateClear(NULL);
}

// Stop communication on the serial port
void endFunction()
{
  K_line.end();
}

// Send a byte
void sendFunction(uint8_t data)
{
  K_line.write(data);
}

// Receive a byte
bool receiveFunction(uint8_t *data, unsigned long timeout_ticks)
{
  // If no byte is immediately available, wait.
  if (K_line.available() <= 0)
  {
    // Wait until either a notification is received, or a timeout occurs.
    if (xTaskNotifyWait(0, 0, NULL, timeout_ticks) != pdTRUE)
    {
      return false;
    }
  }

  // A byte is available; store it in the variable passed by pointer.
  *data = K_line.read();
  return true;
}

// Debugging can be enabled in configuration.h in order to print bus traffic on the Serial Monitor.
#if debug_traffic
void KWP1281debugFunction(bool direction, uint8_t message_sequence, uint8_t message_type, uint8_t *data, size_t length)
{
  printf("\t%s S:%02X T:%02X L:%d ", (direction ? "RECEIVE" : "SEND"), message_sequence, message_type, length);

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
