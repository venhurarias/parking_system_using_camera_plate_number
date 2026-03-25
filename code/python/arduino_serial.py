import serial
import time
import serial.tools.list_ports



def find_arduino_port():
    # Get a list of all available ports
    ports = serial.tools.list_ports.comports()

    # Iterate through the list of ports and check if any of them are Arduino devices
    for port in ports:
        if 'Arduino' in port.description:
            return port.device

    # Return None if no Arduino device is found
    return None

baud_rate = 9600  # Must match the baud rate used in your Arduino sketch

# Initialize the serial connection

arduino_port = find_arduino_port()

if arduino_port:
    ser = serial.Serial(arduino_port, baud_rate, timeout=1)

    try:
        # Main loop
        while True:
            # Send data to Arduino
            ser.write(b'Hello from Python\n')
            time.sleep(1)  # Wait for 1 second

            # Read data from Arduino
            response = ser.readline().decode().strip()
            print("Response from Arduino:", response)

    except KeyboardInterrupt:
        # Close the serial connection when Ctrl+C is pressed
        ser.close()
        print("Serial connection closed.")
