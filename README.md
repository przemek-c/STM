## Additional Checks
If the above change doesn't solve your issue, consider these additional steps:

1. Verify the SPI configuration in your STM32 project:

    * Ensure the SPI is configured with the correct mode (typically Mode 0 or 3 for ICM20948)
    * Check SPI clock speed (should be ≤ 7MHz for ICM20948)

2. Check hardware connections:

    * Confirm proper CS pin wiring
    * Verify MISO, MOSI, and CLK connections
    * Check power supply to the sensor (typically 1.8V or 3.3V)

3. Add a delay after startup before attempting to communicate with the sensor.

This modification should help establish proper timing for the SPI read operation, allowing the sensor to respond with its correct device ID.