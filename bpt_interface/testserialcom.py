import cmd_list as bpt
import serial


def write_dut(str):
    dut.write(str + '\n')
    response = dut.readlines()
    if ('successfully' in response[1]):
        return True
    return False


def read_dut(str):
    print(str + '\n')
    dut.write(str + '\n')
    response = dut.readlines()
    if (len(response) is 6):
        response = response[3][3:len(response[3]) - 4]
        return response
    return None


def init(dev, speed):
    return write_dut('init_master %d %d' % (dev, speed))


def read_reg(addr, reg, size):
    str = 'rr %d %d %d\n' % (addr, reg, size)
    return read_dut(str)


def read_bytes(addr, size):
    str = 'r %d %d\n' % (addr, size)
    return read_dut(str)


def write_reg(addr, reg, data):
    write_str = 'wr %d %d' % (addr, reg)
    for i in len(data):
        write_str += ' %d' % (data[i])
    write_str += '\n'
    return write_dut(write_str)


def write_data(addr, data):
    write_str = 'w %d' % (addr)
    for i in len(data):
        write_str += ' %d' % (data[i])
    write_str += '\n'
    return write_dut(write_str)


bpt.open()

PORT = '/dev/ttyACM1'
BAUD = 115200
dut = serial.Serial(PORT, BAUD, timeout=1)
if (dut.isOpen() is False):
    dut.open()

print(bpt.read_byte(0, 256))

init(0, 1)
print(init(0, 1))

print(read_reg(0x55, 12, 4))


dut.close()
bpt.close()
