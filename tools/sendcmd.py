# Usage example: python3 sendcmd.py -l -c set_controller -d '{"name": "teleop"}'

import common, json

if __name__ == '__main__':

    parser = common.get_argparser('sendcmd.py - send a json command over socket')
    parser.add_argument('-c', '--cmd', help='the command to send', default="none")
    parser.add_argument('-d', '--data', help='', default="{}")
    args = common.parse_args(parser)
    client_sock = common.client_socket_from_args(args)

    data = ''
    try:
        data = json.dumps(json.loads(args['data']))
    except:
        print('Invalid JSON data')
        exit(1)

    client_sock.send(common.create_cmd(args['cmd'], data))