import tkinter as tk

class TwoWidgetsApp:
    def __init__(self, master):
        self.master = master
        master.title("Two Widgets")

        # Create the first widget (80% of the screen width)
        self.widget1 = tk.Frame(master, bg="blue")
        self.widget1_percent = 0.8
        self.widget1.place(relx=0, rely=0, relwidth=self.widget1_percent, relheight=1)

        # Create the second widget (20% of the screen width)
        self.widget2 = tk.Frame(master, bg="red")
        self.widget2_percent = 0.2
        self.widget2.place(relx=self.widget1_percent, rely=0, relwidth=self.widget2_percent, relheight=1)

root = tk.Tk()
app = TwoWidgetsApp(root)
root.mainloop()
