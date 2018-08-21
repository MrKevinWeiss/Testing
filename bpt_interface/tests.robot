*** Settings ***
Test Setup          Reset DUT and BPT

Library             OperatingSystem  
Library             TestUartDevice   port=%{PORT}   WITH NAME  UART
Library             BPTdevice   port=%{BPT_PORT}    WITH NAME  BPT

Resource            testutil.keywords.robot
Resource            expect.keywords.robot
Resource            periph.keywords.robot

Variables           test_vars.py
Variables           uart_test_vars.py

*** Test Cases ***
Echo Test
    ${result}=          BPT.Setup uart
    Should Be True      ${result}

    ${result}=          UART.Send     ${SHORT_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${SHORT_TEST_STRING}

    ${result}=          UART.Send     ${LONG_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${LONG_TEST_STRING}

    ${result}=          BPT.Setup uart    baudrate=38400
    Should Be True      ${result}

    ${result}=          UART.Send     ${SHORT_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${EMPTY}

Extended Echo Test
    ${result}=          BPT.Setup uart     mode=1
    Should Be True      ${result}

    ${result}=          UART.Send     ${SHORT_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${SHORT_TEST_STRING_INC}

    ${result}=          UART.Send     ${LONG_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${LONG_TEST_STRING_INC}

Register Access Test
    ${result}=          BPT.Setup uart     mode=2
    Should Be True      ${result}

    ${result}=          UART.Send     ${REG_152_READ}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${REG_152_READ_DATA}

Even Parity Test
    ${result}=          BPT.Setup uart     parity=${UART_PARITY_EVEN}
    Should Be True      ${result}

    ${result}=          UART.Setup         parity=${UART_PARITY_EVEN}
    Should Be True      ${result}

    ${result}=          UART.Send     ${LONG_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${LONG_TEST_STRING}

Odd Parity Test
    ${result}=          BPT.Setup uart     parity=${UART_PARITY_ODD}
    Should Be True      ${result}

    ${result}=          UART.Setup         parity=${UART_PARITY_ODD}
    Should Be True      ${result}

    ${result}=          UART.Send     ${LONG_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${LONG_TEST_STRING}

Wrong Parity Test
    ${result}=          BPT.Setup uart     parity=${UART_PARITY_EVEN}
    Should Be True      ${result}

    ${result}=          UART.Setup         parity=${UART_PARITY_ODD}
    Should Be True      ${result}

    ${result}=          UART.Send     ${LONG_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${EMPTY}

Two Stop Bits Test
    ${result}=          BPT.Setup uart     stopbits=${UART_STOPBITS_TWO}
    Should Be True      ${result}

    ${result}=          UART.Setup         stopbits=${UART_STOPBITS_TWO}
    Should Be True      ${result}

    ${result}=          UART.Send     ${LONG_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${LONG_TEST_STRING}

RTS On Test
    ${result}=          BPT.Setup uart     rts=True
    Should Be True      ${result}

    ${result}=          UART.Setup         rtscts=True
    Should Be True      ${result}

    ${result}=          UART.Send     ${LONG_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${LONG_TEST_STRING}

RTS Off Test
    ${result}=          BPT.Setup uart     rts=False
    Should Be True      ${result}

    ${result}=          UART.Setup         rtscts=True
    Should Be True      ${result}

    ${result}=          UART.Send     ${LONG_TEST_STRING}
    Should Be True      ${result}
    ${result}=          UART.Read
    Should Contain      ${result}     ${EMPTY}

CTS Test
    ${result}=          BPT.Setup uart     baudrate=921600  mode=3
    Should Be True      ${result}

    ${result}=          UART.Setup         baudrate=921600  rtscts=True
    Should Be True      ${result}

    Sleep               2s

    ${result}=          BPT.Get uart status
    Should Be True      ${result['data']} == 1

    ${result}=          BPT.Setup uart
    Should Be True      ${result}
