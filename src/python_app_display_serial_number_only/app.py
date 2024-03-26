#!/usr/bin/python3
# Python script to parse data from arduino serial port

import time
from init_serial_connection import find_microcontroller, acknowledgement
from consl import console, stat
from rich.table import Table, Column
from rich.panel import Panel

# Sections identifiers
PROGRAM_INIT_MESSAGE = "BATTERY_DIAGNOSTICS_TOOL"
GENERIC_BATTERY_DATA_START = "GENERIC_BATTERY_DATA_START"
GENERIC_BATTERY_DATA_END = "GENERIC_BATTERY_DATA_END"
CELL_DATA_START = "CELL_DATA_START"
CELL_DATA_END = "CELL_DATA_END"
MIN_MAX_DELTA_START = "MIN_MAX_DELTA_START"
MIN_MAX_DELTA_END = "MIN_MAX_DELTA_END"
PROTECTIONS_DATA_START = "PROTECTIONS_DATA_START"
PROTECTIONS_DATA_END = "PROTECTIONS_DATA_END"
REPORT_DATA_START = "REPORT_DATA_START"
REPORT_DATA_END = "REPORT_DATA_END"
BATTERY_SN_START = "BATTERY_SERIAL_NUMBER_START"
BATTERY_SN_STOP = "BATTERY_SERIAL_NUMBER_STOP"


BAUD_RATE = 9600


def main():
    # live.update("[bold red]Welcome to Battery diagnostics tool[/bold red]")

    # The different types of data being recorded
    generic_data = {}
    cell_voltages = {}
    min_max_delta_data = {}
    protections_data = []
    report_data = []
    battery_sn = ''

    recording = True
    recording_generic_data = False
    recording_cell_voltage = False
    recording_min_max_delta = False
    recording_protection_data = False
    recording_report_data = False
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
    prev_time = time.time()
    while recording and time.time() - prev_time < 45:  # 45 seconds
        stat.update()

        line_bytes = microcontroller.readline()
        message = line_bytes.decode(encoding='ascii').strip()

        # Toggle recording flag
        if message == BATTERY_SN_START:
            record_battery_sn = True
        elif message == BATTERY_SN_STOP:
            record_battery_sn = False
            # We are done with recording, so we can display the data
            battery_sn_panel = Panel(
                battery_sn,
                title="[bold cyan]Battery Serial Number[/bold cyan]",
                padding=1
            )
            console.print(battery_sn_panel)

        message_parts = message.split("=")

        if record_battery_sn:
            stat.update("Got battery serial number")
            battery_sn = message_parts[0]

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
