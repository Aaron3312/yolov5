from tkinter import *
import socket




import tkinter as tk
from tkinter import *
import socket
import cv2
from PIL import Image, ImageTk


#ESP_IP = "192.168.13.36"
ESP_IP = "192.168.17.20"
ESP_PORT = 8266
## 

#cap = cv2.VideoCapture('http://10.43.38.8:4747/video')  # Video URL
cap = cv2.VideoCapture(0)  # Video URL



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
lbl_title.grid(row=0, column=0, pady=0, padx=0)
lbl_title.config(font=("Comic_sans", 13))
lbl_title.config(bg="black", fg="white")
lbl_title.config(width=20, height=2)





button_up = Button(frame, text="Arriba W", width=10, height=2, command=lambda: on_button_press("w"))
button_down = Button(frame, text="Abajo S", width=10, height=2, command=lambda: on_button_press("s"))
button_left = Button(frame, text="Izquierda A", width=10, height=2, command=lambda: on_button_press("a"))
button_right = Button(frame, text="Derecha D", width=10, height=2, command=lambda: on_button_press("d"))
button_stop = Button(frame, text="stop Q", width=10, height=2, command=lambda: on_button_press("q"))
button_despacito = Button(frame, text="pasitos_atras E", width=10, height=2, command=lambda: on_button_press("e"))
button_despacito_atras = Button(frame, text="pasitos_adelante - X", width=15, height=2, command=lambda: on_button_press("x"))
button_despacito_drch = Button(frame, text="pasitos_derecha - f", width=15, height=2, command=lambda: on_button_press("f"))
button_despacito_izq = Button(frame, text="pasitos_izquierda - g", width=15, height=2, command=lambda: on_button_press("g"))


button_despacito_atras.grid(row=1, column=0, pady=10)
button_despacito.grid(row=1, column=2, pady=10)
button_up.grid(row=1, column=1, pady=10)
button_down.grid(row=3, column=1, pady=10)
button_left.grid(row=2, column=0, padx=10)
button_right.grid(row=2, column=2, padx=10)
button_stop.grid(row=2, column=1, pady=10)
button_despacito_drch.grid(row=3, column=0, pady=10)
button_despacito_izq.grid(row=3, column=2, pady=10)

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
