import time
import serial
from consl import console, stat, STATUS_WARNING, STATUS_SUCCESS
from serial.tools.list_ports_windows import comports
from serial.serialutil import SerialException


def find_microcontroller(
        start_message: str,
        baud_rate: int = 9600,
        timeout: int = 2) -> serial.Serial | None:
    """Detects and connects to a microcontroller, via Serial ports.
    The microcontroller has to adhere to a specific protocol, which
    is sending a 'HELLO' message on start"""

    msg = ''
    msg = msg + "[bold red]Searching for a microcontroller[/bold red]\n"
    stat.update(msg)

    available_ports = comports()

    for port, desc, hwid in available_ports:

        # Skip bluetooth serial ports
        if "bluetooth" in desc.lower():
            continue

        status_str = f"\
            [bold green]Trying port[/bold green] [bold red]{port}[/bold red]\n\n"
        msg = msg + status_str
        stat.update(msg)

        try:
            ser = serial.Serial(port, baud_rate, timeout=timeout)

            time.sleep(0.1)

            message = ser.readline().decode(
                errors='ignore',
                encoding='ascii').strip()

            if message == start_message:
                # Found microcontroller that implements our protocol
                msg = msg + f"Connected to microcontroller on port {port}"

                console.print(msg)
                # stat.stop()
                # Keep status messages on screen
                return ser
            else:
                ser.close()
        except SerialException as e:
            pass

        time.sleep(1)
    # Keep status messages on screen
    console.print(msg)
    stat.stop()

    return None


def acknowledgement(device: serial.Serial) -> bool:
    """Sends confirmation of successful connection between script and
    the microcontroller"""

    try:
        device.write("CONNECTED".encode('ascii'))
        time.sleep(0.1)
        response = device.readline().decode('ascii').strip()
    except Exception as e:
        print(e)

    if response == "HANDSHAKE_COMPLETE":
        console.style = STATUS_SUCCESS
        console.print("Handshake complete.")
        console.print('Connection Established')
        return True

    else:
        console.style = STATUS_WARNING
        console.print("Handshake failed.")
        console.print("Shutting down")

    return False
