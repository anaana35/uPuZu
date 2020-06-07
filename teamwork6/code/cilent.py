import time
from socket import *

serverName = '192.168.43.241'
serverPort = 6789
while(True):
	clientSocket = socket(AF_INET, SOCK_STREAM)
	clientSocket.connect((serverName, serverPort))
	outputdata = 'temperature'
	clientSocket.send(outputdata.encode())
	data = 1
	data = clientSocket.recv(1024)
	print(data.decode(), end='')
	print('\r')