import tkinter as tk

class MyApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.gripper = tk.Scale(self, from_=0, to=90, orient=tk.HORIZONTAL, command=self.print_value)
        self.gripper.pack()

    def print_value(self, value):
        print(value)

app = MyApp()
app.mainloop()