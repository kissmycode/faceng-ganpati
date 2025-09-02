# udp_client_tester.py
import socket
import time

# --- Configuration ---
# IMPORTANT: Set this to the IP address of the Mac running the wifi_reader.py server.
# You can use "127.0.0.1" if you are running this script on the SAME Mac.
SERVER_IP = "" 
SERVER_PORT = 8888
# ---------------------

def main():
    """
    A simple UDP client to test the wifi_reader.py server.
    Sends 'play_aarti' or 'stop_aarti' commands based on user input.
    """
    # Create a UDP socket (SOCK_DGRAM is for UDP)
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print(f"UDP Client ready to send commands to {SERVER_IP}:{SERVER_PORT}")
    print("Press Ctrl+C to exit at any time.")

    while True:
        print("\n--- Menu ---")
        print("1: Send 'play_aarti'")
        print("2: Send 'stop_aarti'")
        print("3: Loop 5 times")
        print("4: Quit")
        
        choice = input("Enter your choice (1/2/3): ")
        
        command = None
        if choice == '1':
            command = "play_aarti"
        elif choice == '2':
            command = "stop_aarti"
        elif choice == '3':
            command = "loop"
        elif choice == '4':
            break
        else:
            print("Invalid choice. Please try again.")
            continue
        
        try:
            if (command == "loop"):
                loop_cmd_play = "play_aarti"
                loop_cmd_stop = "stop_aarti"
                for i in (0,5):
                    # Encode the command string to bytes and send it to the server
                    client_socket.sendto(loop_cmd_play.encode('utf-8'), (SERVER_IP, SERVER_PORT))
                    print(f"✅ Sent command: '{loop_cmd_play}'")
                    time.sleep(3)
                    client_socket.sendto(loop_cmd_stop.encode('utf-8'), (SERVER_IP, SERVER_PORT))
                    print(f"✅ Sent command: '{loop_cmd_stop}'")
                    time.sleep(3)
            else:
                # Encode the command string to bytes and send it to the server
                client_socket.sendto(command.encode('utf-8'), (SERVER_IP, SERVER_PORT))
                client_socket.sendto(command.encode('utf-8'), (SERVER_IP, SERVER_PORT))
                client_socket.sendto(command.encode('utf-8'), (SERVER_IP, SERVER_PORT))
                client_socket.sendto(command.encode('utf-8'), (SERVER_IP, SERVER_PORT))
                print(f"✅ Sent command: '{command}'")
        except Exception as e:
            print(f"❌ Error sending command: {e}")

    client_socket.close()
    print("\nClient shut down.")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nClient shut down by user.")