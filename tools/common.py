import argparse, socket

def get_argparser(desc, def_port=8000):
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument('-l', '--localhost', help='target localhost as addr', default=False, action='store_true')
    parser.add_argument('-a', '--addr', help='the IPv4 address of the target', default="10.93.24.4") 
    parser.add_argument('-p', '--port', help='the port to target', default=str(def_port)) 
    return parser

def parse_args(parser):
    return vars(parser.parse_args())

def client_socket_from_args(args):
    client_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    addr = args['addr']
    if args['localhost']: addr = 'localhost'
    client_sock.connect((addr, int(args['port'])))
    return client_sock

def create_cmd(cmd, data):
    return str.encode('[' + cmd + ']' + data)