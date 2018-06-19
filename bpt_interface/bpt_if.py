import base_if
import serial.tools.list_ports
import logging


class BptIf(base_if.BaseIf):

    def __init__(self, port=None, baud=115200):

        if (port is None):
            self.autoconnect()
        else:
            self.connect(port, baud)

    def autoconnect(self):
        found_connection = False
        comlist = serial.tools.list_ports.comports()
        connected = []
        logging.debug("Autoconnecting")
        for element in comlist:
            connected.append(element.device)
        for port in connected:
            logging.debug("Port: " + port)
            self.connect(port)
            ret = self.get_device_num().data
            if (isinstance(ret, int)):
                logging.debug("ID: %s" % ret)
                if (ret == 0x42A5):
                    logging.debug("Found connection")
                    found_connection = True
                    break
            self.disconnect()
        return found_connection

    def get_sn(self):
        return self.read_bytes(0, 12)

    def get_fw_rev(self):
        return self.read_bytes(12, 4)

    def get_build_time(self):
        cmd_info = self.read_bytes(16, 8)
        return cmd_info

    def get_device_num(self):
        return self.read_bytes(24, 4)

    def get_i2c_addr_1(self):
        return self.read_bytes(32 + 6, 2)

    def set_i2c_addr_1(self, data=0x55):
        return self.write_bytes(32 + 6, data, 2)

    def get_i2c_addr_2(self):
        return self.read_bytes(32 + 8, 2)

    def set_i2c_addr_2(self, data=0x7F):
        return self.write_bytes(32 + 8, data, 2)

    def get_i2c_10_bit_addr(self):
        return self.read_bits(32, 1, 1)

    def set_i2c_10_bit_addr(self, data=0):
        return self.write_bits(32, 1, 1, data)

    def get_command_list(self):
        cmds = list()
        cmds.append(self.get_sn)
        cmds.append(self.get_fw_rev)
        cmds.append(self.get_build_time)
        cmds.append(self.get_device_num)
        cmds.append(self.set_i2c_addr_1)
        cmds.append(self.get_i2c_addr_1)
        cmds.append(self.get_i2c_addr_2)
        cmds.append(self.set_i2c_addr_2)
        cmds.append(self.get_i2c_10_bit_addr)
        cmds.append(self.set_i2c_10_bit_addr)
        return cmds
