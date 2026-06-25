import serial
import threading
import sys

if len(sys.argv) < 2:
    print("Usage: python main.py <serial_port>")
    sys.exit(1)

serial_port = sys.argv[1]
BAUD = 115200


def read_from_arduino(ser):
    """Continuously read from Arduino and print output."""
    while True:
        try:
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)  # read all available bytes
                try:
                    print(data.decode(errors="replace"), end="")
                except:
                    print(data)
        except Exception as e:
            print("Read error:", e)
            break


def main():
    ser = serial.Serial(serial_port, BAUD, timeout=0.1)

    # give Arduino time to reset
    import time
    time.sleep(2)

    # start background thread for reading
    thread = threading.Thread(target=read_from_arduino,
                              args=(ser,), daemon=True)
    thread.start()

    print("Enter numbers 0–127 to send. Ctrl+C to exit.")

    try:
        while True:
            user_input = input().strip()

            if not user_input:
                continue

            if user_input.lower() in ("exit", "quit"):
                break

            try:
                value = int(user_input)

                if not (0 <= value <= 127):
                    print("Please enter a number between 0 and 127.")
                    continue

                ser.write(bytes([value]))  # send single byte
            except ValueError:
                print("Invalid input. Enter an integer 0–127.")

    except KeyboardInterrupt:
        pass
    finally:
        ser.close()
        print("\nSerial closed.")


if __name__ == "__main__":
    main()
