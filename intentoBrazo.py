from tkinter import *
import socket




import tkinter as tk
from tkinter import *
import socket
import cv2
from PIL import Image, ImageTk


#ESP_IP = "192.168.13.36"
ESP_IP = "192.168.106.20"
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
    
def send_value(servo_id, value):
    # Crear una cadena de texto con el formato que tu ESP espera
    message = f"{servo_id},{value}\n"
    # Enviar la cadena de texto a través del socket
    s.send(message.encode())

    # Aquí puedes enviar el valor donde necesites



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

def print_value(self):
    print(self.gripper.get())
    self.label.config(text=self.gripper.get())
    self.after(100, self.print_value)




root = Tk()
root.title("Control de Carro")

frame = Frame(root)


lbl_title = Label(frame, text="Control de Carro ROBOT y brazo ROBOT")
lbl_title.grid(row=0, column=0, pady=10, padx=10)

#necesito anadir una barra dezlisante para el brazo en el eje z de 0 a 90 valores


gripper = Scale(frame, from_=0, to=90, orient=HORIZONTAL, command=lambda value: send_value(1, gripper.get()))
gripper.grid(row=0, column=1, pady=10, padx=10)


textGripper = Button(frame, text="Gripper", width=10, height=2)
textGripper.grid(row=1, column=1, pady=10, padx=10)
elbow = Scale(frame, from_=0, to=90, orient=HORIZONTAL, command=lambda value: send_value(2,elbow.get()))
elbow.grid(row=0, column=2, pady=10, padx=10)
elbowtext = Button(frame, text="Elbow", width=10, height=2)
elbowtext.grid(row=1, column=2, pady=10, padx=10)

shoulder = Scale(frame, from_=0, to=90, orient=HORIZONTAL, command=lambda value: send_value(3, shoulder.get()))
shoulder.grid(row=0, column=3, pady=10, padx=10)
shouldertext = Button(frame, text="Shoulder", width=10, height=2)
shouldertext.grid(row=1, column=3, pady=10, padx=10)

wrist = Scale(frame, from_=0, to=90, orient=HORIZONTAL, command=lambda value: send_value(4, wrist.get()))
wrist.grid(row=0, column=4, pady=10, padx=10)
wristtext = Button(frame, text="Wrist", width=10, height=2)
wristtext.grid(row=1, column=4, pady=10, padx=10)


button_up = Button(frame, text="Arriba", width=10, height=2, command=lambda: on_button_press("s"))
button_down = Button(frame, text="Abajo", width=10, height=2, command=lambda: on_button_press("w"))
button_left = Button(frame, text="Izquierda", width=10, height=2, command=lambda: on_button_press("a"))
button_right = Button(frame, text="Derecha", width=10, height=2, command=lambda: on_button_press("d"))
button_stop = Button(frame, text="stop", width=10, height=2, command=lambda: on_button_press("q"))

button_up.grid(row=2, column=1, pady=10)
button_down.grid(row=4, column=1, pady=10)
button_left.grid(row=3, column=0, padx=10)
button_right.grid(row=3, column=2, padx=10)
button_stop.grid(row=3, column=1, pady=10)

root.bind("<ButtonRelease-1>", on_button_release)

video_label = Label(root)
video_label.pack()


#imprimir el valor de la barra deslizante en la consola







frame.bind ("<KeyPress>", key_pressed)
frame.bind ("<KeyRelease>", key_released)

s.connect((ESP_IP, ESP_PORT))

frame.pack()
frame.focus_set()
show_frame()


root.mainloop()
