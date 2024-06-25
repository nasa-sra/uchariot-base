import common, json, time

def drive(left, right):
    data = json.dumps({'left_speed': left, 'right_speed': right})
    client_sock.send(common.create_cmd('teleop_drive', data))

def setController(controller):
    data = json.dumps({'name': controller})
    client_sock.send(common.create_cmd('set_controller', data))

def invalid():
    print('Invalid input, stopping robot...')
    drive(0, 0)

if __name__ == '__main__':

    parser = common.get_argparser('teleopderiver.py - drive uChariot in teleop mode')
    args = common.parse_args(parser)
    client_sock = common.client_socket_from_args(args)
    running = True
    while running:

        user = input("> ")

        if user == 'stop':
            print('Stopping robot...')
            drive(0, 0)

        elif user == 'exit':
            print('Exiting...')
            drive(0, 0)
            time.sleep(1)
            running = False

        elif user == 'teleop':
            print('Setting controller to teleop')
            setController('teleop')
        
        elif user == 'disable':
            print('Setting controller to empty')
            setController('empty')

        elif user.startswith('drive'):
            try:
                left, right = user.replace('drive', '').strip().split(' ')
                drive(float(left), float(right))
                print('Driving with left speed {} and right speed {}...'.format(left, right))
            except:
                invalid()
               

        elif user.startswith('turn'):
            try:
                direction, speed = user.replace('turn', '').strip().split(' ')

                if direction == 'left':
                    drive(-float(speed), float(speed))
                    print('Turning to the left with speed {}...'.format(speed))
                elif direction == 'right':
                    drive(float(speed), -float(speed))
                    print('Turning to the right with speed {}...'.format(speed))
                else:
                    invalid() 

            except:
                invalid() 

        else:
            invalid() 