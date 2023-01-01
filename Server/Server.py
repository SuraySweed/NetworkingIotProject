import socket
import sys

PORT = 8820
ServerIP = "127.0.0.1"
ERROR = "ERROR HAS OCCURED"

def main():
    listening_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
     # Bind the socket to the port
    server_address = (ServerIP, PORT)
    print("\nWELCOME TO THE SERVER !!\n-----------------------")
    print ("starting up on port", server_address)
    listening_socket.bind(server_address)
    
    # Listen for incoming connections
    listening_socket.listen(3)

    while (True):
        try:
            # Wait for a connection
            print("waiting for a connection\naccepting client...\n-----------------------")
            connection, client_address = listening_socket.accept()
            print('connection from: ', client_address)
            
            # reading messages
            while (True):
                msg_from_client = connection.recv(2048)
                if (msg_from_client):
                    print("Received: ", msg_from_client.decode('utf-8'))
                else:
                    print("no more data from ", client_address)
                    print("-----------------------")
                    break

        finally:
            connection.close()
            listening_socket.close()

    listening_socket.close()

main()