import asyncio
import websockets
from init_serial_connection import find_microcontroller, acknowledgement
from consl import console, stat
from rich.table import Table, Column
from rich.panel import Panel
# Sections identifiers
PROGRAM_INIT_MESSAGE = "BATTERY_DIAGNOSTICS_TOOL"
BATTERY_SN_START = "BATTERY_SERIAL_NUMBER_START"
BATTERY_SN_STOP = "BATTERY_SERIAL_NUMBER_STOP"
BAUD_RATE = 9600


async def send_battery_sn_to_server(battery_sn):
    async with websockets.connect('ws://localhost:8765') as websocket:
        # Send battery_sn to the WebSocket server
        await websocket.send(battery_sn)


def main():
    # live.update("[bold red]Welcome to Battery diagnostics tool[/bold red]")
    # The different types of data being recorded
    battery_sn = ''
    recording = True
    record_battery_sn = False
    stat.start()
    microcontroller = find_microcontroller(PROGRAM_INIT_MESSAGE)
    if not microcontroller:
        console.print("Microcontroller not found.", style="bold red")
        return False
    elif microcontroller:
        confirmation = acknowledgement(microcontroller)
        if not confirmation:
            console.print("Acknowledgement Failed", style="bold red")
            return False
    # exit()
    console.style = "white"  # reset color theme
    # Continue after successful connection
    while recording:  # 45 seconds
        stat.update()
        line_bytes = microcontroller.readline()
        message = line_bytes.decode(encoding='ascii').strip()
        print(line_bytes, message)
        # Toggle recording flag
        if message == BATTERY_SN_START:
            record_battery_sn = True
            print(f"Received {BATTERY_SN_START}")
            continue
        elif message == BATTERY_SN_STOP:
            record_battery_sn = False
            # We are done with recording, so we can display the data
            battery_sn_panel = Panel(
                battery_sn,
                title="[bold cyan]Battery Serial Number[/bold cyan]",
                padding=1
            )
            console.print(battery_sn_panel)
            asyncio.run(send_battery_sn_to_server(battery_sn))
            recording = False
            continue
        if record_battery_sn:
            stat.update("Got battery serial number")
            battery_sn = message
    stat.stop()
    # Close the serial connection when done
    microcontroller.close()


if __name__ == "__main__":
    # live.start()
    result = main()
    # live.stop()
    print("Done")
    quit_program = input("Press 'Q' to quit and 'O' to keep window open: ")
    if quit_program == 'q':
        print("Exiting")
        exit()
    elif quit_program == 'o':
        import os
        os.system("pause")
