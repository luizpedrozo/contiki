import socket
import struct
import random

HOST = '' #all interfaces
UDP_PORT = 8802

sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) #UDP
sock.bind((HOST, UDP_PORT))

LED_TOGGLE_REQUEST = (0x79)
LED_SET_STATE = (0x7A)
LED_GET_STATE = (0x7B)
LED_STATE = (0x7C)

print("Inciando Servidor")

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print "\n\n====[Recebendo MSG]===="
    print "IP Origem: [", addr[0].strip(),"]"
    print "Porta Origem: [",addr[1], "]"
    print "Mensagem: [", data, "]"
    offset = 0
    op = struct.unpack_from(">B", data,offset)
    offset += struct.calcsize(">B")