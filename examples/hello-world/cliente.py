
import socket
UDP_PORT = 8802
UDP_IP = "127.0.0.1"
MESSAGE = "Minha MSG"
print "Meu IP:", UDP_IP
print "Porta: :", UDP_PORT
print "Mensagem:", MESSAGE
# socket.SOCK_DGRAM = UDP connection
sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
