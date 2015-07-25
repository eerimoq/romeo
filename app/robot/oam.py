#!/usr/bin/env python

import sys
import re
import serial
import threading
import time
import struct
import Queue as queue
import argparse
import readchar


EMTP_MESSAGE_BEGIN = "\x10"


def reader_main(robot_serial):
    """Read data from robot and print it on stdout.
    """

    while True:
        data = robot_serial.read()

        # read message
        if data == EMTP_MESSAGE_BEGIN:
            message_type = robot_serial.read(1)
            message_size = robot_serial.read(2)
            size = struct.unpack('>H', message_size)[0]
            message_data = ''
            data_size = size - 4
            if data_size > 0:
                message_data = robot_serial.read(data_size)
            message = message_type + message_size + message_data
            #sys.stdout.write('Got message: ' + message.encode('hex') + "\r\n")
        else:
            sys.stdout.write(data)
        sys.stdout.flush()


def writer_main(robot_serial, input_queue):
    """Writes data to the robot.
    """

    while True:
        item = input_queue.get()
        robot_serial.write(item)


def ping_main(writer_input_queue):
    """Send ping peridically.
    """

    while True:
        time.sleep(1.0)
        ping_message = "\x00\x00\x04"
        writer_input_queue.put(EMTP_MESSAGE_BEGIN + ping_message)


def start_threads(robot_serial):
    """Start the three threads.
    """
    writer_input_queue = queue.Queue()

    reader_thread = threading.Thread(target=reader_main,
                                     args=(robot_serial, ))
    reader_thread.setDaemon(True)
    reader_thread.start()

    writer_thread = threading.Thread(target=writer_main,
                                     args=(robot_serial, writer_input_queue))
    writer_thread.setDaemon(True)
    writer_thread.start()

    ping_thread = threading.Thread(target=ping_main,
                                   args=(writer_input_queue, ))
    ping_thread.setDaemon(True)
    ping_thread.start()

    return writer_input_queue


def main(args):
    robot_serial = serial.Serial(port=args.serial_port, baudrate=38400)

    writer_input_queue = start_threads(robot_serial)

    sys.stdout.write("Romeo - operation and maintenance.\n")
    sys.stdout.write("ctrl-d to exit\n")
    sys.stdout.write("ctrl-p to begin a message\n")

    message = None

    while True:
        character = readchar.readchar()

        # break on ctrl-d
        if character == '\x04':
            break

        # messages starts with ctrl-p
        if character == "\x10":
            message = ''
            continue

        if message is not None:
            if character == "\x0d":
                raw_message = "\x10" + message.strip().decode('hex')
                writer_input_queue.put(raw_message)
                sys.stdout.write("\r\n")
                message = None
            else:
                sys.stdout.write(character)
                message += character
        else:
            if character == "\x0d":
                writer_input_queue.put("\r\n")
            else:
                writer_input_queue.put(character)

                
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--serial-port", default="/dev/ttyUSB0")
    args = parser.parse_args()
    main(args)
