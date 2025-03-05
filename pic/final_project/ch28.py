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
    if (selection == 'a'):
        pass
    elif (selection == 'b'):
        pass
    elif (selection == 'c'):  # read encoder counts
        # Read serial from PIC32
        encoder_str = ser.read_until(b'\n')
        # convert to int
        encoder_int = int(encoder_str)
        print(encoder_int)
    elif (selection == 'd'):
        pass
    elif (selection == 'e'):  # reset encoder
        print('Encoder reset')
    elif (selection == 'f'):
        pass
    elif (selection == 'g'):
        pass
    elif (selection == 'h'):
        pass
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
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True  # exit client
        ser.close()  # be sure to close the port
    elif (selection == 'r'):
        pass
    else:
        print('Invalid Selection ' + selection_endline)
