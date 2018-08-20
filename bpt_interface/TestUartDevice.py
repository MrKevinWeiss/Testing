"""
Robot framework wrapper.
"""

from robot.version import get_version
import serial


class TestUartDevice(object):
    """Robot framework wrapper class."""

    ROBOT_LIBRARY_SCOPE = 'TEST SUITE'
    ROBOT_LIBRARY_VERSION = get_version()

    def __init__(self, port=None):
        self.dev = serial.serial_for_url(
            port, baudrate=115200, timeout=1)

    def setup(self, baudrate=115200, parity=serial.PARITY_NONE,
              stopbits=serial.STOPBITS_ONE, rtscts=False):
        """Setup DUT UART."""
        self.dev.baudrate = baudrate
        self.dev.parity = parity
        self.dev.stopbits = stopbits
        self.dev.rtscts = rtscts
        self.dev.reset_input_buffer()
        self.dev.reset_output_buffer()

        return True

    def send(self, data):
        """Send data to serial port."""
        ret = True

        data = data.strip('\"') + '\n'
        try:
            self.dev.write(data.encode('utf-8'))
        except serial.SerialTimeoutException:
            ret = False
        except serial.SerialException:
            ret = False

        return ret

    def read(self):
        """Read data from serial port."""
        try:
            buf = self.dev.readline()
        except serial.SerialException as err:
            raise err

        return buf.decode('utf-8').rstrip('\n')
