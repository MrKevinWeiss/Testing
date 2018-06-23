#!/usr/bin/python

# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4

import logging
import argparse
from bpt_if import BptIf

import serial

class RiotProtocolTest(object):

    def __init__(self, port=None, dut_port=None):
        self.bpt = BptIf(port=port)
        self.dev = serial.serial_for_url(dut_port, baudrate=115200, timeout=1)

    def echo_test(self):
        # set UART mode to echo
        cmd_info = self.bpt.set_uart_mode()
        if cmd_info.result != 'Success':
            logging.error("Failed to set UART mode: {}".format(cmd_info.result))
            return False

        cmd_info = self.bpt.execute_changes()
        if cmd_info.result != 'Success':
            logging.error("Failed to excute changes: {}".format(cmd_info.result))
            return False

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err.msg)
            return False

        if len(result) != 3:
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        for item in result:
            logging.debug(item)

        return True

    def echo_ext_test(self):
        # set UART mode to echo_ext
        cmd_info = self.bpt.set_uart_mode(1)
        if cmd_info.result != 'Success':
            logging.error("Failed to set UART mode: {}".format(cmd_info.result))
            return False

        cmd_info = self.bpt.execute_changes()
        if cmd_info.result != 'Success':
            logging.error("Failed to excute changes: {}".format(cmd_info.result))
            return False

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err.msg)
            return False

        if len(result) != 3:
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != b'\x31\x31\x0a':
            logging.error("Wrong string received")
            return False

        for item in result:
            logging.debug(item)

        return True


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", help='Set the logging level (DEBUG, INFO, WARN)')
    parser.add_argument("--port", help='Serial port for BPT communication')
    args = parser.parse_args()

    if args.log is not None:
        loglevel = args.log
        numeric_level = getattr(logging, loglevel.upper(), None)
        if not isinstance(numeric_level, int):
            raise ValueError('Invalid log level: %s' % loglevel)
        logging.basicConfig(level=loglevel)

    test = RiotProtocolTest(dut_port='/dev/ttyUSB1')
    if test.echo_test():
        logging.info("Echo test: OK")
    else:
        logging.info("Echo test: failed")

    if test.echo_ext_test():
        logging.info("Echo extended test: OK")
    else:
        logging.info("Echo extended test: failed")


if __name__ == "__main__":
    main()
