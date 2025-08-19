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
    while True:
        # Wait for a message to arrive
        message, address = server_socket.recvfrom(1024)
        
        # Decode the message from bytes to a string
        command = message.decode('utf-8').strip()
        
        print(f"Received command '{command}' from {address}")
        
        if command == "play_aarti":
            try:
                # Find and play the track
                for track in apple_music.tracks.get():
                    if track.name.get() == "Sukhkarta_aarti":
                        track.play()
                        aarti_found = True
                        print(f"Now playing: {track.name.get()}")
                        break
            except Exception as e:
                print(f"Error playing track: {e}")
        elif command == "stop_aarti":
            apple_music.stop();
            print(f"Aarti stopped");

except KeyboardInterrupt:
    print("\nServer is shutting down.")
finally:
    server_socket.close()
    print("Socket closed.")