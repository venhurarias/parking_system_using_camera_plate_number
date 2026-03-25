import sqlite3
import tkinter as tk

def create_table():
    conn = sqlite3.connect('database.db')
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS history (
            id INTEGER PRIMARY KEY,
            tenant_id TEXT,
            time_in TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    ''')
    conn.commit()
    conn.close()

def populate_list():
    # Clear the listbox before repopulating
    listbox.delete(0, tk.END)
    
    # Fetch data from the database and populate the listbox
    conn = sqlite3.connect('database.db')
    cursor = conn.cursor()
    cursor.execute('SELECT * FROM history')
    rows = cursor.fetchall()
    for row in rows:
        listbox.insert(tk.END, row)
    conn.close()

# Create the table
create_table()

# Create the main window
root = tk.Tk()
root.title("List UI Example")

# Make the window full screen
root.attributes('-fullscreen', True)

# Create a frame to hold the label and the list
frame = tk.Frame(root)
frame.pack(fill=tk.BOTH, expand=True)

# Create a label at center top
label = tk.Label(frame, text="History")
label.pack(side=tk.TOP, pady=10)

# Create a listbox with padding
listbox = tk.Listbox(frame)
listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=20, pady=20)

# Create a scrollbar
scrollbar = tk.Scrollbar(frame, orient=tk.VERTICAL, command=listbox.yview)
scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
listbox.config(yscrollcommand=scrollbar.set)

# Button to populate the list
populate_button = tk.Button(root, text="Populate List", command=populate_list)
populate_button.pack(pady=5)

# Start the Tkinter event loop
root.mainloop()
