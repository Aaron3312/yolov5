import cv2
import urllib.request
import numpy as np
import threading

# Function to read frames from the stream
def read_frames(stream, buffer_size=1024):
    bytes = b''
    while True:
        bytes += stream.read(buffer_size)
        a = bytes.find(b'\xff\xd8')  # frame starting
        b = bytes.find(b'\xff\xd9')  # frame ending
        if a != -1 and b != -1:
            jpg = bytes[a:b + 2]
            bytes = bytes[b + 2:]

            if not jpg:
                continue

            img = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)

            if img is not None:
                # Display the image in the main thread
                cv2.imshow('image', img)

                if cv2.waitKey(1) == 27:
                    cv2.destroyAllWindows()
                    break

# Main function
def main():
    stream = urllib.request.urlopen('http://192.168.17.162:80/1024x768.mjpeg')

    # Use a larger buffer size (adjust as needed)
    buffer_size = 1

    # Create a separate thread for reading frames
    thread = threading.Thread(target=read_frames, args=(stream, buffer_size))
    thread.start()

    # Wait for the thread to finish
    thread.join()

if __name__ == "__main__":
    main()
