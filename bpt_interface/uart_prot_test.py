#!/usr/bin/python

# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4

import time
import logging
import argparse
from enum import Enum

import serial

from bpt_mem_map import BptMemMap


class BptUartModes(Enum):
    '''BPT UART test modes.'''
    ECHO = 0
    ECHO_EXT = 1
    REG_ACCESS = 2
    TX = 3


class RiotProtocolTest(object):

    def autoconnect(self, used_com=[]):
        logging.debug("Autoconnecting")
        logging.debug("Set DUT UART to echo mode")
        cmd_info = self.bpt.set_uart_mode()
        if cmd_info.result != 'Success':
            logging.error(
                "Failed to set UART mode: {}".format(cmd_info.result))
            return None

        cmd_info = self.bpt.set_uart_baud(115200)
        if cmd_info.result != 'Success':
            logging.error(
                "Failed to set UART mode: {}".format(cmd_info.result))
            return None

        cmd_info = self.bpt.execute_changes()
        if cmd_info.result != 'Success':
            logging.error(
                "Failed to excute changes: {}".format(cmd_info.result))
            return None

        found_connection = False
        comlist = serial.tools.list_ports.comports()
        connected = []
        for element in comlist:
            if (element.device not in used_com):
                connected.append(element.device)
        for port in connected:
            logging.debug("Port: " + port)
            self.dev = serial.serial_for_url(
                port, baudrate=115200, timeout=0.1)
            expected_val = b'\x30\x30\x0a'
            self.dev.write(expected_val)
            ret = self.dev.readline()
            try:
                logging.debug("ID rx: %s" % ret)
                logging.debug("ID expected: %s" % expected_val)
                if (ret == expected_val):
                    logging.debug("Found connection")
                    found_connection = True
                    break
            except TypeError:
                logging.debug("Cannot connect, type error")
            except ValueError:
                logging.debug("Cannot connect, value error")
            except Exception as e:
                logging.debug(e)
            self.dev.close()

        return found_connection

    def __init__(self, port=None, dut_port=None):
        self.bpt = BptMemMap(port=port)
        if dut_port is None:
            self.autoconnect(used_com=[self.bpt.get_port()])
        else:
            self.dev = serial.serial_for_url(
                dut_port, baudrate=115200, timeout=1)

    def setup_dut_uart(self, baudrate=115200, parity=serial.PARITY_NONE,
                       stopbits=serial.STOPBITS_ONE, rtscts=False):
        '''Setup DUT UART.'''
        self.dev.baudrate = baudrate
        self.dev.parity = parity
        self.dev.stopbits = stopbits
        self.dev.rtscts = rtscts
        self.dev.reset_input_buffer()
        self.dev.reset_output_buffer()

    def setup_bpt(self, mode=BptUartModes.ECHO.value, baudrate=115200,
                  parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                  rts=True):

        '''Setup BPT.'''
        # setup testing mode
        cmd_info = self.bpt.set_uart_mode(mode)
        if cmd_info.result != 'Success':
            logging.error(
                "Failed to set UART mode: {}".format(cmd_info.result))
            return False

        # setup baudrate
        cmd_info = self.bpt.set_uart_baud(baudrate)
        if cmd_info.result != 'Success':
            logging.error(
                "Failed to set UART mode: {}".format(cmd_info.result))
            return False

        # setup UART control register
        ctrl = 0
        if parity == serial.PARITY_EVEN:
            ctrl = ctrl | 0x02
        elif parity == serial.PARITY_ODD:
            ctrl = ctrl | 0x04

        if stopbits == serial.STOPBITS_TWO:
            ctrl = ctrl | 0x01

        # invert RTS level as it is a low active signal
        if not rts:
            ctrl = ctrl | 0x08

        cmd_info = self.bpt.set_uart_ctrl(ctrl)
        if cmd_info.result != 'Success':
            logging.error(
                "Failed to set UART control: {}".format(cmd_info.result))
            return False

        # reset status
        cmd_info = self.bpt.set_uart_status(0)
        if cmd_info.result != 'Success':
            logging.error(
                "Failed to set UART status: {}".format(cmd_info.result))
            return False

        # apply changes
        cmd_info = self.bpt.execute_changes()
        if cmd_info.result != 'Success':
            logging.error(
                "Failed to excute changes: {}".format(cmd_info.result))
            return False

        return True

    def echo_test(self):
        if not self.setup_bpt():
            return False

        self.setup_dut_uart()

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        rx = self.bpt.get_uart_rx_count()
        tx = self.bpt.get_uart_tx_count()
        logging.debug("RX {}, TX {}".format(rx.data, tx.data))

        return True

    def echo_test_with_hw_cf(self):
        if not self.setup_bpt(rts=True):
            return False

        self.setup_dut_uart(rtscts=True)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        rx = self.bpt.get_uart_rx_count()
        tx = self.bpt.get_uart_tx_count()
        logging.debug("RX {}, TX {}".format(rx.data, tx.data))

        return True

    def echo_test_with_hw_cf_rts_low(self):
        if not self.setup_bpt(rts=False):
            return False

        self.setup_dut_uart(rtscts=True)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            return True

        if result != test_str:
            return False

        logging.debug('Received string: {}'.format(result))

        rx = self.bpt.get_uart_rx_count()
        tx = self.bpt.get_uart_tx_count()
        logging.debug("RX {}, TX {}".format(rx.data, tx.data))

        return False

    def cts_signal_test(self):
        if not self.setup_bpt(baudrate=921600, mode=BptUartModes.TX.value):
            return False

        self.setup_dut_uart(baudrate=921600, rtscts=True)

        time.sleep(2)

        status = self.bpt.get_uart_status()

        if status.data != 1:
            return False

        if not self.setup_bpt():
            return False

        return True

    def echo_test_wrong_baudrate(self):
        if not self.setup_bpt(baudrate=9600):
            return False

        self.setup_dut_uart()

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.debug("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return True

        if result != test_str:
            logging.debug("Wrong string received")
            return True

        logging.debug('Received string: {}'.format(result))

        return False

    def echo_test_even_parity(self):
        if not self.setup_bpt(parity=serial.PARITY_EVEN):
            return False

        self.setup_dut_uart(parity=serial.PARITY_EVEN)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        rx = self.bpt.get_uart_rx_count()
        tx = self.bpt.get_uart_tx_count()
        logging.debug("RX {}, TX {}".format(rx.data, tx.data))

        return True

    def echo_test_odd_parity(self):
        if not self.setup_bpt(parity=serial.PARITY_ODD):
            return False

        self.setup_dut_uart(parity=serial.PARITY_ODD)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        rx = self.bpt.get_uart_rx_count()
        tx = self.bpt.get_uart_tx_count()
        logging.debug("RX {}, TX {}".format(rx.data, tx.data))

        return True

    def echo_test_wrong_parity(self):
        if not self.setup_bpt(parity=serial.PARITY_EVEN):
            return False

        self.setup_dut_uart(parity=serial.PARITY_ODD)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.debug("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return True

        if result != test_str:
            logging.debug("Wrong string received")
            return True

        logging.debug('Received string: {}'.format(result))

        return False

    def echo_test_two_stopbits(self):
        if not self.setup_bpt(baudrate=600, stopbits=serial.STOPBITS_TWO):
            return False

        self.setup_dut_uart(baudrate=600, stopbits=serial.STOPBITS_TWO)

        test_str = b'\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        rx = self.bpt.get_uart_rx_count()
        tx = self.bpt.get_uart_tx_count()
        logging.debug("RX {}, TX {}".format(rx.data, tx.data))

        return True

    def echo_ext_test(self):
        if not self.setup_bpt(mode=BptUartModes.ECHO_EXT.value):
            return False

        self.setup_dut_uart()

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != b'\x31\x31\x0a':
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        rx = self.bpt.get_uart_rx_count()
        tx = self.bpt.get_uart_tx_count()
        logging.debug("RX {}, TX {}".format(rx.data, tx.data))

        return True

    def reg_read_test(self):
        if not self.setup_bpt(mode=BptUartModes.REG_ACCESS.value):
            return False

        self.setup_dut_uart()

        test_str = 'rr 152 10\n'
        try:
            self.dev.write(test_str.encode('utf-8'))
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if result != b'0,0x09080706050403020100\n':
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        rx = self.bpt.get_uart_rx_count()
        tx = self.bpt.get_uart_tx_count()
        logging.debug("RX {}, TX {}".format(rx.data, tx.data))

        return True


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--log", help='Set the logging level (DEBUG, INFO, WARN)')
    parser.add_argument("--port", help='Serial port for BPT communication')
    parser.add_argument("--dutport", help='Serial port for DUT communication')
    args = parser.parse_args()

    if args.log is not None:
        loglevel = args.log
        numeric_level = getattr(logging, loglevel.upper(), None)
        if not isinstance(numeric_level, int):
            raise ValueError('Invalid log level: %s' % loglevel)
        logging.basicConfig(level=loglevel)

    test = RiotProtocolTest(port=args.port, dut_port=args.dutport)

    if test.echo_test():
        logging.info("Echo test: OK")
    else:
        logging.info("Echo test: failed")

    if test.echo_test_with_hw_cf():
        logging.info("Echo test hw fc: OK")
    else:
        logging.info("Echo test hw fc: failed")

    if test.echo_test_with_hw_cf_rts_low():
        logging.info("Echo test hw fc RTS low: OK")
    else:
        logging.info("Echo test hw fc RTS low: failed")

    if test.echo_test_wrong_baudrate():
        logging.info("Echo test wrong baudrate: OK")
    else:
        logging.info("Echo test wrong baudrate: failed")

    if test.echo_test_even_parity():
        logging.info("Echo test with even parity: OK")
    else:
        logging.info("Echo test with even parity: failed")

    if test.echo_test_odd_parity():
        logging.info("Echo test with odd parity: OK")
    else:
        logging.info("Echo test with odd parity: failed")

    if test.echo_test_wrong_parity():
        logging.info("Echo test with wrong parity: OK")
    else:
        logging.info("Echo test with wrong parity: failed")

    if test.echo_test_two_stopbits():
        logging.info("Echo test with two stopbits: OK")
    else:
        logging.info("Echo test with two stopbits: failed")

    if test.echo_ext_test():
        logging.info("Echo extended test: OK")
    else:
        logging.info("Echo extended test: failed")

    if test.reg_read_test():
        logging.info("Register read test: OK")
    else:
        logging.info("Register read test: failed")

    if test.cts_signal_test():
        logging.info("CTS signal test: OK")
    else:
        logging.info("CTS signal test: failed")


if __name__ == "__main__":
    main()
