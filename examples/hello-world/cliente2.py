
import socket
import struct

UDP_PORT = 8802
UDP_IP = "2804:7f4:3b80:9440:5748:f6e0:f733:4c6b"

LED_TOGGLE_REQUEST = (0x79)
LED_SET_STATE = (0x7A)
LED_GET_STATE = (0x7B)
LED_STATE = (0x7C)

MESSAGE = struct.pack(">B", LED_TOGGLE_REQUEST)
print "IP Servidor:", UDP_IP
print "Porta: :", UDP_PORT
print "Mensagem enviada para servidor:", MESSAGE
# socket.SOCK_DGRAM = UDP connection
sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))

print("Aguardando resposta do servidor...")
while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print "\n\n====[Recebendo MSG]===="
    print "IP Origem: [", addr[0].strip(),"]"
    print "Porta Origem: [",addr[1], "]"
    print "Mensagem: [", data, "]"
    offset = 0
    op = struct.unpack_from(">B", data,offset)
    offset += struct.calcsize(">B")

    print("Enviar resposta ao servidor")
    MESSAGE = struct.pack(">B", LED_STATE)
    MESSAGE += struct.pack(">B", offset)
    print "IP Servidor:", UDP_IP
    print "Porta: :", UDP_PORT
    print "Mensagem enviada para servidor:", MESSAGE
    # socket.SOCK_DGRAM = UDP connection
    sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
    break


