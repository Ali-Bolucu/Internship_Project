
import time
import socket
from parameters import UserValues

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	try:
		s.connect((UserValues.HOST_ADRESS,UserValues.HOST_PORT))
		print(f"Connected to {UserValues.HOST_ADRESS}")
		while True:
        	
			data = input("Enter a string: ")
			s.sendall(data.encode())
			
			if (data == "close") or (data == "exit"):
				print("closing")
				s.close()
				
        	
			#s.sendall(b"Hello, world")
			data = s.recv(512).decode()
			print(f"[server] {data}")
			#if data == "CHECK":
			#s.sendall("OK".encode())
                
	except:
		s.close()  

