# chapter 28 in python

# Menu options:
#
# a: Read current sensor(ADC counts)
# b: Read current sensor(mA)
# c: Read encoder(counts)
# d: Read encoder(deg)
# e: Reset encoder
# f: Set PWM(-100 to 100)
# g: Set current gains(Kp, Ki)
# h: Get current gains(Kp, Ki)
# i: Set position gains(Kp, Ki)
# j: Get position gains(Kp, Ki)
# k: Test current control
# l: Go to angle(deg)
# m: Load step trajectory
# n: Load cubic trajectory
# o: Execute trajectory
# p: Unpower the motor
# q: Quit client
# r: Get mode

import serial
ser = serial.Serial('/dev/ttyUSB0', 230400)
print('Opening port: ')
print(ser.name)

MENU_STR = """\
\ta: Read current sensor (ADC counts)      b: Read current sensor (mA)
\tc: Read encoder (counts)                 d: Read encoder (deg)
\te: Reset encoder                         f: Set PWM (-100 to 100)
\tg: Set current gains (Kp, Ki)            h: Get current gains (Kp, Ki)
\ti: Set position gains (Kp, Ki)           j: Get position gains (Kp, Ki)
\tk: Test current control                  l: Go to angle (deg)
\tm: Load step trajectory                  n: Load cubic trajectory
\to: Execute trajectory                    p: Unpower the motor
\tq: Quit client                           r: Get mode
"""


MODE_MAP = {
    0: 'IDLE',
    1: 'PWM',
    2: 'ITEST',
    3: 'HOLD',
    4: 'TRACK'
}

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options
    print(MENU_STR)
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'

    # send the command to the PIC32
    # .encode() turns the string into a char array
    ser.write(selection_endline.encode())

    # take the appropriate action
    if (selection == 'a'):  # read current sensor (ADC counts)
        print('Read ADC Counts is not supported')
    elif (selection == 'b'):  # read current sensor (mA)
        mA_str = ser.read_until(b'\n')
        mA_float = float(mA_str)
        print(mA_float)
    elif (selection == 'c'):  # read encoder counts
        encoder_str = ser.read_until(b'\n')
        encoder_int = int(encoder_str)
        print(encoder_int)
    elif (selection == 'd'):  # read encoder degrees
        encoder_str = ser.read_until(b'\n')
        encoder_float = float(encoder_str)
        print(encoder_float)
    elif (selection == 'e'):  # reset encoder
        print('Encoder reset')
    elif (selection == 'f'):  # set PWM (-100 to 100)
        duty_cycle = input('Enter duty cycle (-100 to 100): ')
        duty_cycle_int = int(duty_cycle)
        ser.write((str(duty_cycle_int)+'\n').encode())
        pwm_str = ser.read_until(b'\n')
        pwm_float = float(pwm_str)
        print(
            f'Set PWM duty cycle to {abs(pwm_float)}% with direction {"CW" if pwm_float > 0 else "CCW"}')
    elif (selection == 'g'):  # set current gains (Kp, Ki)
        Kp = input('Enter Kp: ')
        ser.write((Kp+'\n').encode())
        Ki = input('Enter Ki: ')
        ser.write((Ki+'\n').encode())
        gains_str = ser.read_until(b'\n')  # 'Kp Ki'
        gains = gains_str.split()
        Kp_float = float(gains[0])
        Ki_float = float(gains[1])
        print(f'Set current gains Kp={Kp_float} Ki={Ki_float}')
    elif (selection == 'h'):  # get current gains (Kp, Ki)
        gains_str = ser.read_until(b'\n')
        gains = gains_str.split()
        Kp_float = float(gains[0])
        Ki_float = float(gains[1])
        print(f'Current gains Kp={Kp_float} Ki={Ki_float}')
    elif (selection == 'i'):
        pass
    elif (selection == 'j'):
        pass
    elif (selection == 'k'):
        pass
    elif (selection == 'l'):
        pass
    elif (selection == 'm'):
        pass
    elif (selection == 'n'):
        pass
    elif (selection == 'o'):
        pass
    elif (selection == 'p'):
        pass
    elif (selection == 'q'):  # quit client
        print('Exiting client')
        has_quit = True  # exit client
        ser.close()  # be sure to close the port
    elif (selection == 'r'):  # get mode
        mode_str = ser.read_until(b'\n')
        mode_int = int(mode_str)
        print(MODE_MAP[mode_int])
    elif (selection == 'x'):  # testing command
        print('Toggling Green LED')
    else:
        print('Invalid Selection ' + selection_endline)
