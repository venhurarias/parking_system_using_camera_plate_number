import cv2
import tkinter as tk
from tkinter import Label, Frame
from PIL import Image, ImageTk
import sys, os, argparse
import simplelpr
import serial
import time
import serial.tools.list_ports
import sqlite3


class LicensePlateRecognitionApp:
    setupP = simplelpr.EngineSetupParms()
    eng = simplelpr.SimpleLPR(setupP)
    frame_count =0
    ser=serial.Serial()
    prevPlate=""
    verifyCnt=0

    conn = sqlite3.connect('data.db')
    cur = conn.cursor()

    def __init__(self, root):
        self.root = root
        self.root.title("License Plate Recognition")

        # Make the window fullscreen
        self.root.attributes("-fullscreen", True)

        # Initialize camera
        self.cap = cv2.VideoCapture(0)

        # Create UI components
        self.create_widgets()

        if not self.table_exists('tenant'):
            create_table_sql = '''
            CREATE TABLE tenant (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                plate_number TEXT NOT NULL
            );
            '''
            self.cur.execute(create_table_sql)
            self.conn.commit()

            # Insert initial data
            initial_data = [
                ("John Doe", "ABC123"),
                ("Jane Smith", "XYZ789"),
                ("Alice Johnson", "LMN456")
            ]
            self.cur.executemany("INSERT INTO tenant (name, plate_number) VALUES (?, ?)", initial_data)
            self.conn.commit()



        arduino_port = self.find_arduino_port()
        if arduino_port:
            self.ser = serial.Serial(arduino_port, 9600, timeout=1)
            self.update()
            # try:
            #     while True:
            #         # Send data to Arduino
            #         self.ser.write(b'Hello from Python\n')
            #         time.sleep(1)  # Wait for 1 second

            #         # Read data from Arduino
            #         response = self.ser.readline().decode().strip()
            #         print("Response from Arduino:", response)

            # except KeyboardInterrupt:
            #     # Close the serial connection when Ctrl+C is pressed
            #     self.ser.close()
            #     print("Serial connection closed.")



        # Start the update loop
        

        # Bind the escape key to exit fullscreen
        self.root.bind("<Escape>", self.exit_fullscreen)

    def find_arduino_port(self):
        # Get a list of all available ports
        ports = serial.tools.list_ports.comports()

        # Iterate through the list of ports and check if any of them are Arduino devices
        for port in ports:
            if 'Arduino' in port.description:
                return port.device

        # Return None if no Arduino device is found
        return None
    
    def table_exists(self, table_name):
        self.cur.execute("SELECT name FROM sqlite_master WHERE type='table' AND name=?", (table_name,))
        return self.cur.fetchone() is not None

    def create_widgets(self):
        self.widget1 = tk.Frame(self.root,bg="black")
        self.widget1_percent = 0.8
        self.widget1.place(relx=0, rely=0, relwidth=self.widget1_percent, relheight=1)

        self.camera_frame = Label(self.widget1, text="Camera Frame",bg="black")
        self.camera_frame.grid(row=0, column=0, sticky="nsew")  # Set sticky to "nsew"

        # Create the second widget (20% of the screen width)
        self.widget2 = tk.Frame(self.root)
        self.widget2_percent = 0.2
        self.widget2.place(relx=self.widget1_percent, rely=0, relwidth=self.widget2_percent, relheight=1)

        self.instructions = Label(self.widget2, text="Wait for a vehicle...", font=("Arial", 24))
        self.instructions.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")


        
    def update(self):
        # Capture frame-by-frame
        ret, frame = self.cap.read()
        self.frame_count += 1
        if(self.frame_count>=10):
            self.analyze_file(frame)
            self.frame_count=0
        if ret:
            # Get the current size of the camera frame
            frame_width = self.widget1.winfo_width()
            frame_height = self.widget1.winfo_height()

            if frame_width > 0 and frame_height > 0:
                # Resize the frame to fit the camera frame
                frame = cv2.resize(frame, (frame_width, frame_height))

                # Convert the frame to RGB (tkinter requires it)
                frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                img = Image.fromarray(frame)
                imgtk = ImageTk.PhotoImage(image=img)

                # Update the camera frame
                self.camera_frame.imgtk = imgtk
                self.camera_frame.configure(image=imgtk)

        # Schedule the next update
        self.root.after(10, self.update)

    def exit_fullscreen(self, event=None):
        self.root.attributes("-fullscreen", False)
        self.root.quit()

    def on_closing(self):
        # Release the camera and destroy all windows when closing the app
        self.ser.close()
        self.cap.release()
        self.root.destroy()

    def analyze_file(self, frame):
        # Enables syntax verification with the selected country.
        self.eng.set_countryWeight("Philippines", 1)
        self.eng.realizeCountryWeights()

        proc = self.eng.createProcessor()

        # Enable the plate region detection and crop to plate region features.
        proc.plateRegionDetectionEnabled = True
        proc.cropToPlateRegionEnabled = True

        cds = proc.analyze(frame)

        # Show the detection results.
        print('Number of detected candidates:', len(cds))

        if len(cds) > 0:
            confidence=0.00
            plateNumber=""
            for cand in cds:
                print('-----------------------------')
                print('darkOnLight:', cand.darkOnLight, ', plateDetectionConfidence:', cand.plateDetectionConfidence)
                print('boundingBox:', cand.boundingBox)
                print('plateRegionVertices:', cand.plateRegionVertices)

                for cm in cand.matches:
                    print('\tcountry:', "'{:}'".format(cm.country), ', countryISO:', "'{:}'".format(cm.countryISO),
                        ', text:', "'{:}'".format(cm.text), ', confidence:', '{:.3f}'.format(cm.confidence))
                    if(cm.confidence>=confidence and cm.confidence>0.9):
                        
                        plateNumber=cm.text
                        confidence=cm.confidence
                        if self.prevPlate==plateNumber:
                            self.verifyCnt+=1
                            if self.verifyCnt>=3:
                                self.instructions.config(text=plateNumber)
                                

                        else:
                            self.prevPlate=plateNumber
                            self.verifyCnt=0
                        

                    
        else:
            self.instructions.config(text="Wait for a vehicle...")



       


if __name__ == "__main__":
    root = tk.Tk()
    app = LicensePlateRecognitionApp(root)
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    root.mainloop()
