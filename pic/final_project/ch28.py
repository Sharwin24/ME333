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

import matplotlib.pyplot as plt
import serial
import os
from genref import genRef
ser = serial.Serial('/dev/ttyUSB0', 230400)
print('Opening port: ')
print(ser.name)

MENU_STR = """\
\ta: Read current sensor (ADC counts)      b: Read current sensor (mA)
\tc: Read encoder (counts)                 d: Read encoder (deg)
\te: Reset encoder                         f: Set PWM (-100 to 100)
\tg: Set current gains (Kp, Ki)            h: Get current gains (Kp, Ki)
\ti: Set position gains (Kp, Ki, Kd)       j: Get position gains (Kp, Ki, Kd)
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

# Current Control Gains
Kp_ICtrl = 0.0
Ki_ICtrl = 0.0

# Position Control Gains
Kp_Pos = 0.0
Ki_Pos = 0.0
Kd_Pos = 0.0

# Saved traj type
traj_type = 'None'

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
        Kp_ICtrl = float(gains[0])
        Ki_ICtrl = float(gains[1])
        print(f'Set current gains Kp={Kp_ICtrl} Ki={Ki_ICtrl}')
    elif (selection == 'h'):  # get current gains (Kp, Ki)
        gains_str = ser.read_until(b'\n')
        gains = gains_str.split()
        Kp_ICtrl = float(gains[0])
        Ki_ICtrl = float(gains[1])
        print(f'Current gains Kp={Kp_ICtrl} Ki={Ki_ICtrl}')
    elif (selection == 'i'):  # set position gains (Kp, Ki, Kd)
        Kp = input('Enter Kp: ')
        ser.write((Kp+'\n').encode())
        Ki = input('Enter Ki: ')
        ser.write((Ki+'\n').encode())
        Kd = input('Enter Kd: ')
        ser.write((Kd+'\n').encode())
        gains_str = ser.read_until(b'\n')  # 'Kp Ki'
        gains = gains_str.split()
        Kp_Pos = float(gains[0])
        Ki_Pos = float(gains[1])
        Kd_Pos = float(gains[2])
        print(f'Set position gains Kp={Kp_Pos} Ki={Ki_Pos} Kd={Kd_Pos}')
        pass
    elif (selection == 'j'):  # get position gains (Kp, Ki, Kd)
        gains_str = ser.read_until(b'\n')
        gains = gains_str.split()
        Kp_Pos = float(gains[0])
        Ki_Pos = float(gains[1])
        Kd_Pos = float(gains[2])
        print(f'Position gains Kp={Kp_Pos} Ki={Ki_Pos} Kd={Kd_Pos}')
        pass
    elif (selection == 'k'):  # test current control
        gains_str = ser.read_until(b'\n')
        gains = gains_str.split()
        Kp_ICtrl = float(gains[0])
        Ki_ICtrl = float(gains[1])
        print('Testing current control')
        # PIC will send ref and actual value in a loop
        # until 100 samples are taken
        ref_values = []
        actual_values = []
        for i in range(100):
            ref_str = ser.read_until(b'\n')
            ref_float = float(ref_str)
            actual_str = ser.read_until(b'\n')
            actual_float = float(actual_str)
            ref_values.append(ref_float)
            actual_values.append(actual_float)
        meanzip = zip(ref_values, actual_values)
        meanlist = []
        for i, j in meanzip:
            meanlist.append(abs(i-j))
        score = sum(meanlist)/len(meanlist)
        # print('Ref values:', ref_values)
        # print('Actual values:', actual_values)
        # plot the values on the same plot and save the figure
        plt.plot(ref_values, label=f'Reference [mA]')
        plt.plot(actual_values,
                 label=f'Actual [mA] Kp={Kp_ICtrl} Ki={Ki_ICtrl}')
        plt.text(0.95, 0.95, f'Score: {score:.2f}', transform=plt.gca().transAxes, fontsize=12,
                 verticalalignment='top', horizontalalignment='right', bbox=dict(facecolor='white', alpha=0.5),
                 ha='right', va='top')
        plt.legend(loc='lower right')
        plt.xlabel('Sample')
        plt.ylabel('Current [mA]')
        plt.title('Current Control Test')
        plt.savefig(f'current_control_test_{score:.1f}.png')
        plt.close()
    elif (selection == 'l'):  # go to angle (deg)
        gains_str = ser.read_until(b'\n')
        gains = gains_str.split()
        Kp_Pos = float(gains[0])
        Ki_Pos = float(gains[1])
        Kd_Pos = float(gains[2])
        target = input('Enter target angle [deg]: ')
        ser.write((target+'\n').encode())
        print(f'Going to {target} [deg]')
    elif (selection == 'm'):  # load step trajectory
        ref = genRef('step')
        traj_type = 'step'
        num_steps = len(ref)
        ser.write((str(num_steps)+'\n').encode())
        # ref is a list of time and angles (0, 0, 1, 90, 2, 45, 3, 45)
        for i in ref:
            ser.write((str(i)+'\n').encode())
        print(f'Loaded step trajectory with {num_steps} steps')
    elif (selection == 'n'):  # Load cubic trajectory
        ref = genRef('cubic')
        traj_type = 'cubic'
        num_steps = len(ref)
        ser.write((str(num_steps)+'\n').encode())
        # ref is a list of time and angles (0, 0, 1, 90, 2, 45, 3, 45)
        for i in ref:
            ser.write((str(ref[i])+'\n').encode())
        print(f'Loaded cubic trajectory with {num_steps} steps')
    elif (selection == 'o'):  # execute trajectory
        ser.timeout = 3
        gains_str = ser.read_until(b'\n')
        if not gains_str:
            print('Error: No response from PIC32')
            continue
        gains = gains_str.split()
        Kp_Pos = float(gains[0])
        Ki_Pos = float(gains[1])
        Kd_Pos = float(gains[2])
        print(f'Using gains Kp={Kp_Pos} Ki={Ki_Pos} Kd={Kd_Pos}')
        ref_values = []
        actual_values = []
        num_steps_str = ser.read_until(b'\n')
        num_steps = int(num_steps_str)
        print(f'Executing trajectory with {num_steps} steps')
        for i in range(num_steps):
            actual_str = ser.read_until(b'\n')
            ref_str = ser.read_until(b'\n')
            actual_float = float(actual_str)
            ref_float = float(ref_str)
            ref_values.append(ref_float)
            actual_values.append(actual_float)
        # plot the values on the same plot and save the figure
        plt.plot(actual_values, label=f'Actual [deg]')
        plt.plot(ref_values, label=f'Reference [deg]', linestyle='--')
        plt.legend(loc='lower right')
        plt.xlabel('Step')
        plt.ylabel('Angle [deg]')
        plt.text(0.95, 0.95, f'Kp={Kp_Pos} Ki={Ki_Pos} Kd={Kd_Pos}', transform=plt.gca().transAxes, fontsize=12,
                 verticalalignment='top', horizontalalignment='right', bbox=dict(facecolor='white', alpha=0.5),
                 ha='right', va='top')
        plt.title(f'Trajectory Test ({traj_type})')
        plt.savefig(f'trajectory_test.png')
        plt.close()
    elif (selection == 'p'):  # unpower the motor
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
        print('Toggling LED')
    else:
        print('Invalid Selection ' + selection_endline)
