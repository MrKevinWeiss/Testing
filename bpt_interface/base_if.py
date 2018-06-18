import serial
import logging
import errno
import os
import time


class IfParams:
    cmd = ''
    result = ''
    msg = ''
    data = ''


class BaseIf:
    __READ_REG_CMD = "rr"
    __WRITE_REG_CMD = "wr"
    __EXECUTE_CMD = "ex"
    __RESET_CMD = "mcu_rst"
    __SUCCESS = '0'
    __RESULT_SUCCESS = 'Success'
    __RESULT_ERROR = 'Error'
    __RESULT_TIMEOUT = 'Timeout'

    def __init__(self, port='/dev/ttyACM0', baud=115200):
        self.connect(port, baud)

    def connect(self, port, baud=115200, timeout=0.3):
        logging.debug("Connecting to " + port)
        self.__dev = serial.Serial(port, baud, timeout=timeout)
        if(self.__dev.isOpen() is False):
            self.__dev.open()
        logging.debug("Flushing input for 100ms")
        time.sleep(0.1)
        self.__dev.flushInput()

    def disconnect(self):
        logging.debug("Disconnecting")
        if (isinstance(self.__dev, serial.Serial)):
            if (self.__dev.isOpen()):
                self.__dev.close()

    def send_cmd(self, send_cmd):
        cmd_info = IfParams()
        logging.debug("Sending: " + send_cmd)
        self.__dev.write(send_cmd + '\n')
        cmd_info.cmd = send_cmd
        response = self.__dev.readline()
        logging.debug("Response: " + response.replace('\n', ''))
        if (response == ""):
            cmd_info.result = self.__RESULT_TIMEOUT
            logging.debug(self.__RESULT_TIMEOUT)
        else:
            data = response.replace('\n', '')
            data = data.split(',')
            if (data[0] == self.__SUCCESS):
                if (len(data) > 1):
                    try:
                        if (len(data[1]) - 2 <= 8):
                            cmd_info.data = long(data[1], 0)
                        else:
                            d_len = len(data[1]) - 1
                            cmd_info.data = bytearray.fromhex(data[1][2:d_len])
                    except ValueError:
                        self.data = data[1:]
                cmd_info.msg = "EOK-command success [0]"
                logging.debug(self.__RESULT_SUCCESS)
                cmd_info.result = self.__RESULT_SUCCESS
            else:
                cmd_info.data = int(data[0], 0)
                cmd_info.msg = "%s-%s [%d]" % (errno.errorcode[cmd_info.data],
                                               os.strerror(cmd_info.data),
                                               cmd_info.data)
                logging.debug(self.__RESULT_ERROR)
                cmd_info.result = self.__RESULT_ERROR
        return cmd_info

    def read_bytes(self, index, size=1):
        logging.debug("FXN: read_bytes(%d, %d)" % (index, size))
        cmd = '%s %d %d' % (self.__READ_REG_CMD, index, size)
        return self.send_cmd(cmd)

    def write_bytes(self, index, data, size=4):
        logging.debug("FXN: write_bytes(%d, %d)" % (index, data))
        cmd = "%s %d" % (self.__WRITE_REG_CMD, index)
        if (isinstance(data, list)):
            for i in range(0, len(data)):
                if (len(data) - i < len(data)):
                    cmd += ' %d' % (data[len(data) - i])
                else:
                    cmd += ' 0'
        else:
            for i in range(0, size):
                cmd += ' %d' % ((data >> ((i) * 8)) & 0xFF)
        return self.send_cmd(cmd)

    def read_bits(self, index, offset, bit_amount):
        logging.debug("FXN: read_bits(%d, %d, %d)" % (index, offset,
                                                      bit_amount))
        bit_mask = (2 ** bit_amount) - 1
        cmd_info = self.read_bytes(index, (bit_amount - 1)/8 + 1)
        if (cmd_info.result == self.__RESULT_SUCCESS):
            cmd_info.cmd += ',read_bits %d %d %d' % (index, offset, bit_amount)
            cmd_info.data = cmd_info.data >> offset
            cmd_info.data = cmd_info.data & bit_mask
        return cmd_info

    def write_bits(self, index, offset, bit_amount, data):
        cmd_sent = ""
        logging.debug("FXN: write_bits(%d, %d, %d, %d)" % (index, offset,
                                                           bit_amount, data))
        cmd_info = self.read_bytes(index, (bit_amount - 1)/8 + 1)
        if (cmd_info.result != self.__RESULT_SUCCESS):
            return cmd_info
        cmd_sent += cmd_info.cmd
        bit_mask = long((2 ** bit_amount) - 1)
        bit_mask = bit_mask << offset
        cmd_info.data = cmd_info.data & (~bit_mask)
        data = cmd_info.data | ((data << offset) & bit_mask)
        cmd_info = self.write_bytes(index, data)
        cmd_sent += cmd_info.cmd
        if (cmd_info.result == self.__RESULT_SUCCESS):
            cmd_sent += ',write_bits %d %d %d %d' % (index, offset,
                                                     bit_amount, data)
        cmd_info.cmd = cmd_sent
        return cmd_info

    def execute_changes(self):
        logging.debug("FXN: execute_changes")
        return self.send_cmd(self.__EXECUTE_CMD)

    def reset_mcu(self):
        logging.debug("FXN: reset_mcu")
        return self.send_cmd(self.__RESET_CMD)
