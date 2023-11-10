from tkinter import *
import socket




import tkinter as tk
from tkinter import *
import socket
import cv2
from PIL import Image, ImageTk


ESP_IP = "192.168.13.36"
ESP_PORT = 8266
## 

cap = cv2.VideoCapture('http://10.43.40.234:4747/video')  # Video URL



##
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

def key_pressed(c):
    print("Tecla presionada: ", c.char)
    s.send(c.char.encode())
def key_released(c):
    print("Tecla liberada: ", c.char)
    s.send(c.char.encode())



def on_button_press(direction):
    print("Botón presionado: ", direction)
    s.send(direction.encode())
    


def on_button_release(event):
    print("Botón liberado")

def show_frame():
    _, frame = cap.read()
    frame = cv2.resize(frame, (640, 480))
    cv2image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGBA)
    img = Image.fromarray(cv2image)
    imgtk = ImageTk.PhotoImage(image=img)
    video_label.imgtk = imgtk
    video_label.configure(image=imgtk)
    video_label.after(10, show_frame)


root = Tk()
root.title("Control de Carro")

frame = Frame(root)

lbl_title = Label(frame, text="Control de Carro ROBOT")
lbl_title.grid(row=0, column=0, pady=10, padx=10)

button_up = Button(frame, text="Arriba", width=10, height=2, command=lambda: on_button_press("s"))
button_down = Button(frame, text="Abajo", width=10, height=2, command=lambda: on_button_press("w"))
button_left = Button(frame, text="Izquierda", width=10, height=2, command=lambda: on_button_press("a"))
button_right = Button(frame, text="Derecha", width=10, height=2, command=lambda: on_button_press("d"))
button_stop = Button(frame, text="stop", width=10, height=2, command=lambda: on_button_press("q"))

button_up.grid(row=1, column=1, pady=10)
button_down.grid(row=3, column=1, pady=10)
button_left.grid(row=2, column=0, padx=10)
button_right.grid(row=2, column=2, padx=10)
button_stop.grid(row=2, column=1, pady=10)

root.bind("<ButtonRelease-1>", on_button_release)

video_label = Label(root)
video_label.pack()

frame.bind ("<KeyPress>", key_pressed)
frame.bind ("<KeyRelease>", key_released)

s.connect((ESP_IP, ESP_PORT))

frame.pack()
frame.focus_set()
show_frame()


root.mainloop()
