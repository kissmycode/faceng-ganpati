import socket
from appscript import app # For Apple Music control

# --- Configuration ---
# Listen on all available network interfaces
LISTEN_IP = "0.0.0.0" 
LISTEN_PORT = 8888 # Must match the port in the Arduino code
# --- Configuration End ---

# Create a UDP socket
try:
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind((LISTEN_IP, LISTEN_PORT))
    print(f"✅ Server is listening on port {LISTEN_PORT}...")
    print("Press Ctrl+C to stop.")

    # Connect to Apple Music
    apple_music = app('Music')
    print("Connected to Apple Music.")

except Exception as e:
    print(f"Error setting up: {e}")
    exit()

try:
    aarti_found = False;
    aarti_on = False;
    while True:
        # 1. Wait for at least one command to arrive
        message, address = server_socket.recvfrom(1024)
        
        # 2. Quickly drain any other commands waiting in the buffer
        commands = [message]
        server_socket.setblocking(False)
        while True:
            try:
                commands.append(server_socket.recvfrom(1024)[0])
            except BlockingIOError:
                break # Buffer is empty
        server_socket.setblocking(True)

        # 3. Decode and process ONLY the last command received
        command = commands[-1].decode('utf-8').strip()
        print(f"Drained {len(commands)} packets. Processing last command: '{command}'")

        if command == "play_aarti":
            if not aarti_on:
                try:
                    # Find and play the track
                    for track in apple_music.tracks.get():
                        if track.name.get() == "Sukhkarta_aarti":
                            track.play()
                            aarti_found = True
                            aarti_on = True
                            print(f"Now playing: {track.name.get()}")
                            break
                    if not aarti_found:
                        print("Sukhakarta aarti not found")
                        aarti_found = False
                        aarti_on = False
                except Exception as e:
                    print(f"Error playing track: {e}")
            else:
                print("Aarti already on. Ignoring play command"); 
        elif command == "stop_aarti":
            if aarti_on:
                aarti_on = False
                apple_music.stop()
                print("Aarti stopped");
            else:
                print("Aarti already stopped. Ignoring stop command.")

except KeyboardInterrupt:
    print("\nServer is shutting down.")
finally:
    server_socket.close()
    print("Socket closed.")