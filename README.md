# Additional Checks for IMU
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

# UART 

w main.h sprawdzić gdzie wkleić dokładnie

## Kroki konfiguracji
1. Upewnij się, że skonfigurowałeś USART1 w MX Cube:

    * Ustaw odpowiednie piny (PA9 i PA10 dla USART1)
    * Włącz przerwania UART
    * Skonfiguruj baudrate (np. 115200)
    * Upewnij się, że włączone są globalne przerwania
2. Dodaj include do biblioteki USART w pliku main.c

3. Zdefiniuj struktury do odbierania danych przez UART

3. Zainicjuj UART i uruchom odbiór danych w trybie przerwania

3. Zaimplementuj funkcję obsługi przerwania UART

3. Dodaj funkcje do sterowania LED-ami

## Szczegóły implementacji
1. Konfiguracja UART w CubeMX:

    * Włącz USART1 w trybie asynchronicznym (pinach PA9/PA10)
    * Ustaw baudrate: 115200
    * Słowo danych: 8 bitów
    * Brak parzystości
    * 1 bit stopu
    * Włącz przerwania globalne i UART

2. Format komend:

    * "LED1 ON" - włącza LED1
    * "LED1 OFF" - wyłącza LED1
    * Podobnie dla LED2 i LED3

3. Obsługa przerwań UART:

    * Dane są odbierane znak po znaku
    * Po wykryciu znaku końca linii ('\r' lub '\n'), komenda jest przetwarzana
    * System wysyła odpowiedź potwierdzającą wykonanie akcji

4. Sterowanie LED-ami:

    * LED1 znajduje się na pinie PB11
    * LED2 znajduje się na pinie PB12
    * LED3 znajduje się na pinie PB13

Jeśli nie skonfigurowałeś jeszcze USART1, musisz to zrobić w CubeMX i wygenerować kod. Upewnij się, że PIN_PA9 i PIN_PA10 są skonfigurowane jako odpowiednio TX i RX dla USART1.

## Pi part

1. Upewnij się, że UART jest włączony na Raspberry Pi:

    * Użyj sudo raspi-config i włącz Serial Port
    * Alternatywnie edytuj /boot/config.txt i dodaj enable_uart=1
3. Połącz Raspberry Pi z STM32 przez UART:

    * Raspberry Pi UART TX (GPIO14/pin 8) → STM32 RX (PA10)
    * Raspberry Pi UART RX (GPIO15/pin 10) → STM32 TX (PA9)
    * Połącz masę (GND) między urządzeniami

5. Wybierz opcję z menu, aby kontrolować LED-y

Jeśli porty szeregowe są inne niż /dev/ttyAMA0, dostosuj kod odpowiednio. Możesz sprawdzić dostępne porty szeregowe na Raspberry Pi poleceniem:

```bash
ls -l /dev/tty* 
```