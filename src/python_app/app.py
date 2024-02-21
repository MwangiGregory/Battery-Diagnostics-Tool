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

BAUD_RATE = 9600


def main():
    # live.update("[bold red]Welcome to Battery diagnostics tool[/bold red]")

    # The different types of data being recorded
    generic_data = {}
    cell_voltages = {}
    min_max_delta_data = {}
    protections_data = []
    report_data = []

    recording = True
    recording_generic_data = False
    recording_cell_voltage = False
    recording_min_max_delta = False
    recording_protection_data = False
    recording_report_data = False

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
        # print(message)

        if message == GENERIC_BATTERY_DATA_START:
            recording_generic_data = True

        elif message == GENERIC_BATTERY_DATA_END:
            recording_generic_data = False

            # When recording is set to false, we are done with collecting all
            # data and we can display it
            # display generic data headers
            generic_data_table = Table(
                # Destructure the array into positional args passed
                # to the Table object
                *[Column(key) for key in generic_data.keys()],
                header_style="bold cyan"
                # row_styles=["green"]
            )
            # display corresponding generic data values
            generic_data_table.add_row(*[val for val in generic_data.values()],
                                       style="green")
            console.print(generic_data_table)

        elif message == CELL_DATA_START:
            recording_cell_voltage = True
        elif message == CELL_DATA_END:
            recording_cell_voltage = False

            # display cell voltage data
            cell_data_first_15 = Table(
                # get all 30 keys the take an array slice of first 15
                # Then destructure the array into positional args passed
                # to the Table object
                *([Column(key) for key in cell_voltages.keys()][:15]),
                header_style="bold cyan"
            )
            cell_data_first_15.add_row(
                *([val for val in cell_voltages.values()][:15]),
                style="green"
            )

            cell_data_last_15 = Table(
                *([Column(key) for key in cell_voltages.keys()][15:30]),
                header_style="bold cyan"
            )
            cell_data_last_15.add_row(
                *([val for val in cell_voltages.values()][15:30]),
                style="green")

            console.print(cell_data_first_15)
            console.print(cell_data_last_15)

        elif message == MIN_MAX_DELTA_START:
            recording_min_max_delta = True
        elif message == MIN_MAX_DELTA_END:
            recording_min_max_delta = False

            # display min max delta data
            min_max_delta_table = Table(
                *[Column(key) for key in min_max_delta_data.keys()],
                header_style="bold cyan"
            )
            min_max_delta_table.add_row(
                *[val for val in min_max_delta_data.values()],
                style="green"
            )
            console.print(min_max_delta_table)

        elif message == PROTECTIONS_DATA_START:
            recording_protection_data = True
            continue
        elif message == PROTECTIONS_DATA_END:
            recording_protection_data = False

            # display protection data
            protection_data_panel = Panel(
                ''.join(protections_data),
                title="[bold cyan]Protections[/bold cyan]",
                padding=1
            )
            console.print(protection_data_panel)

        elif message == REPORT_DATA_START:
            recording_report_data = True
            continue
        elif message == REPORT_DATA_END:
            recording_report_data = False
            recording = False

            # display report data
            report_data_panel = Panel(
                ''.join(report_data),
                title="[bold cyan]Report[/bold cyan]",
                padding=1
            )
            console.print(report_data_panel)

        message_parts = message.split("=")

        if recording_generic_data:
            stat.update("Recording generic battery data")
            if len(message_parts) > 1:
                key = message_parts[0]
                value = message_parts[1]
                generic_data[key] = value
        elif recording_cell_voltage:
            stat.update("Recording cell voltage data")
            if len(message_parts) > 1:
                key = message_parts[0]
                value = message_parts[1]
                cell_voltages[key] = value
        elif recording_min_max_delta:
            stat.update("Recording min max delta cell voltge data")
            if len(message_parts) > 1:
                key = message_parts[0]
                value = message_parts[1]
                min_max_delta_data[key] = value
        elif recording_protection_data:
            stat.update("Recording protections data")
            if message_parts[0] not in protections_data:
                protections_data.append(message_parts[0])
        elif recording_report_data:
            stat.update("Recording report data")
            if message_parts[0] not in report_data:
                report_data.append(message_parts[0])

    stat.stop()

    # if generic_data:
    #     print("***********GENERIC_DATA***********")
    #     print(generic_data)
    #     print("***********GENERIC_DATA***********")
    #     print()
    # if cell_voltages:
    #     print("***********CELL_VOLTAGES***********")
    #     print(cell_voltages)
    #     print("***********CELL_VOLTAGES***********")
    #     print()

    # if min_max_delta_data:
    #     print("***********MIN_MAX_DELTA***********")
    #     print(min_max_delta_data)
    #     print("***********MIN_MAX_DELTA***********")
    #     print()

    # if protections_data:
    #     print("***********PROTECTION***********")
    #     print(protections_data)
    #     print("***********PROTECTION***********")
    #     print()

    # if report_data:
    #     print("***********REPORT***********")
    #     print(report_data)
    #     print("***********REPORT***********")
    #     print()

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
