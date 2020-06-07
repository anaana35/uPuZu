# import socket module
from socket import *
import json
import time

serverSocket = socket(AF_INET, SOCK_STREAM)
# Prepare a sever socket
serverSocket.bind(('localhost', 6789))  # 将TCP欢迎套接字绑定到指定端口
serverSocket.listen(1)  # 最大连接数为1

return_data = {'data': '', 'temperature': '0.0', 'humidity': '0.0'}

while True:
    # Establish the connection
    print('Ready to serve...')
    connectionSocket, addr = serverSocket.accept()  # 接收到客户连接请求后，建立新的TCP连接套接字
    try:
        message = connectionSocket.recv(1024).decode()  # 获取客户发送的报文
        print(message)
        raw_data = ""
        if message == 'temperature':
            f = open('./cache.txt')
            raw_data = f.readline()
            f.close()
            local_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
            return_data['data'] = local_time
            return_data['temperature'] = raw_data.split()[0]
            return_data['humidity'] = raw_data.split()[1]
            connectionSocket.send(json.dumps(return_data).encode())
            connectionSocket.close()
        else:
            msg = 'no commend'
            connectionSocket.send(msg.encode())

            # Close client socket
            connectionSocket.close()

    except IOError:
        # Send response message for file not found
        header = ' HTTP/1.1 404 Found'
        connectionSocket.send(header.encode())
    finally:
        # Close client socket
        connectionSocket.close()
