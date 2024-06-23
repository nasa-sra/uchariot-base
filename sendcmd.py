# Usage example: python3 sendcmd.py -l -c set_controller -d '{"name": "teleop"}'

import socket, argparse, json

parser = argparse.ArgumentParser(description='sendcmd.py - send a json command over socket')
parser.add_argument('-l', '--localhost', help='target localhost as addr', default=False, action='store_true')
parser.add_argument('-a', '--addr', help='the IPv4 address of the target', default="10.0.0.1") 
parser.add_argument('-p', '--port', help='the port to target', default="8000") 
parser.add_argument('-c', '--cmd', help='the command to send', default="none")
parser.add_argument('-d', '--data', help='', default="{}")
args = vars(parser.parse_args())

client_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
addr = args['addr']
if args['localhost']: addr = 'localhost'
client_sock.connect((addr, int(args['port'])))

data = ''
try:
    data = json.dumps(json.loads(args['data']))
except:
    print('Invalid JSON data')

    
to_send = '[' + args['cmd'] + ']' + data
print(to_send)
client_sock.send(str.encode(to_send))