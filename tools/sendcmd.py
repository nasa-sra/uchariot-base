# Usage example: python3 sendcmd.py -l -c set_controller -d '{"name": "teleop"}'
# python3 sendcmd.py -l -c run_path -d '{"name": "AutonPath", "res": "1"}'
# python3 sendcmd.py -l -c set_controller -d '{"name": "pathing"}'
# python3 sendcmd.py -l -c run_path -d '{"name": "AutonPath", "res": "1", "speed": "2500", "rad": "3"}'
import common, json, socket

if __name__ == '__main__':

    parser = common.get_argparser('sendcmd.py - send a json command over socket')
    parser.add_argument('-c', '--cmd', help='the command to send', default="none")
    parser.add_argument('-d', '--data', help='', default="{}")
    args = common.parse_args(parser)
    common.client_socket_from_args
    client_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_sock.connect(('10.93.24.5', 8001))

    data = ''
    try:
        data = json.dumps(json.loads(args['data']))
    except:
        print('Invalid JSON data')
        exit(1)

    client_sock.send(common.create_cmd(args['cmd'], data))