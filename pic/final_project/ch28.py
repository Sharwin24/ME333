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

MENU_STR = '\ta: Read curent sensor (ADC counts) \t b: Read current sensor (mA) \n\
            \tc: Read encoder (counts) \t d: Read encoder (deg) \n\
            \te: Reset encoder \t f: Set PWM (-100 to 100) \n\
            \tg: Set current gains (Kp, Ki) \t h: Get current gains (Kp, Ki) \n\
            \ti: Set position gains (Kp, Ki) \t j: Get position gains (Kp, Ki) \n\
            \tk: Test current control \t l: Go to angle (deg) \n\
            \tm: Load step trajectory \t n: Load cubic trajectory \n\
            \to: Execute trajectory \t p: Unpower the motor \n\
            \tq: Quit client \t r: Get mode \n'

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

    # elif (selection == 'x'):
    #     # example operation
    #     n_str = input('Enter number: ')  # get the number to send
    #     n_int = int(n_str)  # turn it into an int
    #     # print it to the screen to double check
    #     print('number = ' + str(n_int))

    #     ser.write((str(n_int)+'\n').encode())  # send the number
    #     n_str = ser.read_until(b'\n')  # get the incremented number back
    #     n_int = int(n_str)  # turn it into an int
    #     print('Got back: ' + str(n_int) + '\n')  # print it to the screen
