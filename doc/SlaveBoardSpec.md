# Purpose

To have a low cost, certified slave device that can be used to automate regression testing of basic peripherals.

# Problems

- Sensors and other slave devices cannot test all configurable modes
- RPi does not support low level testing
- Standard Arduino extensions don't support all configurable features well
- Standard test tools from DIOLAN and Total Phase require licenses and are expensive

# Requirements

- Operate with 3.3V boards and 5V boards
- Reasonably low cost
- Can be build/ordered by anyone
- SPI slave with different modes, up to 24 MHz
- I2C slave with different addresses, up to 3.4 MHz, no internal pullup
- UART with modem support, baudrate up to 256000 bps, parity, stop bit
- ADC values can be streamed
- GPIO input
- GPIO output
- CCP with resolution to count ? MHz
- Clock Output of stable 10 1KHz
- Serial Number/Firmware Revision for tractability

# Future Considerations
- CANBus support
- Master mode of periph support
- Multi master I2C

# Implementation
- The board will be based off of a bare metal implementation on a blue pill STM32 board
- The slave board will act as a slave device with a register list similar to sensors such as the bme280 or lis3dh
- Control of the slave device can be done with a combination of GPIO (initially) and SPI
- 1 SPI slave mode for testing that has configurable "mode"
- 1 I2C bus with various addresses and pullup settings
- 1 UART with variable baud, parity, stop, modem Control
- 2 ADC from 0 to 3.3v reading at 96kHz sample
- Use 96 bit UID for SN
- 1 PWM output, configurable period, duty cycle, ramp mode, default 10 kHz 50%
- 1 capture pin that can read high time, low time, period
- 1 input pin that must be toggled?
- Control from UART that is static (baud etc. cannot change)

# Target Tests to Run
## SPI Test
### Pass Cases
- write to dummy register
- read from dummy register
- ensure a change has occurred
- Change to all 4 modes
- 8/16 bit frame
- Speeds
- Stress test
- Send different dummy data
### Failure Cases
- Incorrect mode settings
- Incorrect pin configs
- Unsupported Speeds
- Frame errors

## I2C
### Pass Cases
- Check if slave present
- write register
- read register
- change slave address
- slave clock stretch
- slave data not ready
- speeds
- stress tests
- 10 bit and 7 bit addr

### Failure Cases
- unterminated session
- wrong slave address
- Incorrect pin configs
- Unsupported Speeds
- no pullup resistor

## UART
### Pass Cases
- Basic comms
- Modem support
- change baudrate
- parity
- stop bit
- stress rx
- echo data
- change data
- ack data
- autobaud?

### Failure Cases
- Wrong baudrate
- wrong configs
- Incorrect pin configs

## ADC
### Pass Cases
- Linearity
- speeds

### Failure Cases
- Incorrect pin configs
- Unsupported Speeds

## PWM
### Pass Cases
- Change duty cycle
- Change period
- min/max bounds
- disable/enable
- Timing test

### Failure Cases
- ?

## DAC
### Pass Cases
- Linearity

### Failure Cases
- ?

# Protocol Definition
Register Based, up to 256 registers but not more.  UART will be the primary interface.  It will support basic commands like read and writing register, execute changes, and reset.

## I2C comms
Default slave addr is 0x55, can be changed to 0 to 0x7F minus reserved addr
LSb indicates R/W, read is 1, write is 0
Enable/disable slave clock stretching
