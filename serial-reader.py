import serial
import time
from appscript import app, its

# --- Configuration ---
# Ensure baud_rate matches the baud rate set in your Arduino code
# serial_port = '/dev/cu.usbserial-210'
serial_port = '/dev/tty.HC-05'
baud_rate = 9600
# --- Configuration End ---


def read_arduino_serial(port, baudrate, timeout=5):
    """
    Reads data from the Arduino's serial port.

    Args:
        port (str): The serial port path (e.g., '/dev/cu.usbXXXX').
        baudrate (int): The communication speed in bits per second (bps).
        timeout (int): The maximum time to wait for data in seconds.
    """
    try:
        # Open the serial port
        ser = serial.Serial(port, baudrate, timeout=timeout)
        time.sleep(2)  # Give Arduino time to reset after opening the port

        print(f"Connected to Arduino on port {port} at {baudrate} baud.")
        print("Press Ctrl+C to stop reading.")

        try:
            apple_music = app('Music')
        except Exception as e:
            print(f"Couldn't fetch apple music: {e}")

        aarti_found = False
        while True:
            # Read a line of data, decode it from bytes to string, and remove whitespace
            data = ser.readline().decode('utf-8').strip()
            if data:
                print(f"Received: {data}")
                if data.startswith("play_aarti"):
                    for track in apple_music.tracks.get():
                        if track.name.get() == "Sukhkarta_aarti":
                            track.play();
                            aarti_found = True;
                            print(f"Now playing: {track.name.get()}");
                            break;
                    if not aarti_found:
                        print(f"Did not find aarti");
                elif data.startswith("stop_aarti") and aarti_found:
                    apple_music.stop();
                    print(f"Aarti stopped");
    except serial.SerialException as e:
        print(f"Error: Could not open or communicate with the serial port: {e}")
        print("Please ensure your Arduino is connected and the correct port and baud rate are specified.")
    except KeyboardInterrupt:
        print("\nReading stopped by user.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial port closed.")


if __name__ == '__main__':
    read_arduino_serial(serial_port, baud_rate)