#!/usr/bin/env python3
# pip3 install pyserial

import serial
import time
import sys

def main():
    # Konfiguracja portu szeregowego
    # Raspberry Pi zazwyczaj używa /dev/ttyAMA0 lub /dev/ttyS0 dla UART
    # Zmień na odpowiedni port w zależności od konfiguracji Twojego Raspberry Pi
    try:
        ser = serial.Serial(
            port='/dev/ttyAMA0',   # lub '/dev/ttyS0' w zależności od modelu i konfiguracji
            baudrate=115200,       # ta sama prędkość co na STM32
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1
        )
        print("Port szeregowy otwarty.")
    except serial.SerialException as e:
        print(f"Nie można otworzyć portu szeregowego: {e}")
        sys.exit(1)
        
    # Poczekaj na stabilizację połączenia
    time.sleep(2)
    
    # Opróżnij bufor wejściowy
    ser.flushInput()

    print("\nSterowanie LED-ami na STM32")
    print("==========================")
    
    try:
        while True:
            print("\nMenu:")
            print("1. Włącz LED1")
            print("2. Wyłącz LED1")
            print("3. Włącz LED2")
            print("4. Wyłącz LED2")
            print("5. Włącz LED3")
            print("6. Wyłącz LED3")
            print("7. Wyjście")
            
            choice = input("\nWybierz opcję (1-7): ")
            
            if choice == '7':
                print("Kończenie programu...")
                break
                
            # Mapowanie wyboru na odpowiednie polecenie
            commands = {
                '1': "LED1 ON",
                '2': "LED1 OFF",
                '3': "LED2 ON",
                '4': "LED2 OFF",
                '5': "LED3 ON",
                '6': "LED3 OFF"
            }
            
            if choice in commands:
                command = commands[choice]
                print(f"Wysyłanie polecenia: {command}")
                
                # Dodaj znak nowego wiersza na końcu polecenia
                command += "\r\n"
                
                # Wyślij polecenie do STM32
                ser.write(command.encode('utf-8'))
                
                # Poczekaj i odczytaj odpowiedź
                time.sleep(0.5)
                response = ""
                while ser.in_waiting > 0:
                    response += ser.read(ser.in_waiting).decode('utf-8')
                    time.sleep(0.1)
                
                # Wyświetl odpowiedź jeśli otrzymana
                if response:
                    print(f"Odpowiedź: {response.strip()}")
                else:
                    print("Brak odpowiedzi od STM32")
            else:
                print("Nieprawidłowy wybór, spróbuj ponownie.")
                
    except KeyboardInterrupt:
        print("\nProgram przerwany przez użytkownika.")
    finally:
        # Zamknij port szeregowy
        if ser.is_open:
            ser.close()
            print("Port szeregowy zamknięty.")

if __name__ == "__main__":
    main()
