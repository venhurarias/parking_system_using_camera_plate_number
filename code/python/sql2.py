import tkinter as tk
from tkinter import messagebox, simpledialog
import sqlite3
from datetime import datetime

# Connect to the SQLite database
conn = sqlite3.connect('data.db')
cur = conn.cursor()

# Function to check if a table exists
def table_exists(cursor, table_name):
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table' AND name=?", (table_name,))
    return cursor.fetchone() is not None

# Check if the tenant table exists and create it if not
if not table_exists(cur, 'tenant'):
    create_table_sql = '''
    CREATE TABLE tenant (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        plate_number TEXT NOT NULL
    );
    '''
    cur.execute(create_table_sql)
    conn.commit()

    # Insert initial data
    initial_data = [
        ("John Doe", "ABC123"),
        ("Jane Smith", "XYZ789"),
        ("Alice Johnson", "LMN456")
    ]
    cur.executemany("INSERT INTO tenant (name, plate_number) VALUES (?, ?)", initial_data)
    conn.commit()

# Check if the tenant_log table exists and create it if not
if not table_exists(cur, 'tenant_log'):
    create_log_table_sql = '''
    CREATE TABLE tenant_log (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        tenant_id INTEGER,
        action TEXT NOT NULL,
        timestamp TEXT NOT NULL,
        FOREIGN KEY (tenant_id) REFERENCES tenant (id)
    );
    '''
    cur.execute(create_log_table_sql)
    conn.commit()

# Function to add a tenant
def add_tenant():
    name = name_entry.get()
    plate_number = plate_entry.get()
    if name and plate_number:
        cur.execute("INSERT INTO tenant (name, plate_number) VALUES (?, ?)", (name, plate_number))
        conn.commit()
        tenant_id = cur.lastrowid
        log_action(tenant_id, 'add')
        name_entry.delete(0, tk.END)
        plate_entry.delete(0, tk.END)
        refresh_listbox()
    else:
        messagebox.showwarning("Input Error", "Please enter both name and plate number.")

# Function to edit a tenant
def edit_tenant():
    try:
        selected_id = tenant_listbox.get(tenant_listbox.curselection()).split(':')[0]
        new_name = simpledialog.askstring("Edit Name", "Enter new name:")
        new_plate = simpledialog.askstring("Edit Plate Number", "Enter new plate number:")
        if new_name and new_plate:
            cur.execute("UPDATE tenant SET name = ?, plate_number = ? WHERE id = ?", (new_name, new_plate, selected_id))
            conn.commit()
            log_action(selected_id, 'edit')
            refresh_listbox()
        else:
            messagebox.showwarning("Input Error", "Please enter both name and plate number.")
    except tk.TclError:
        messagebox.showwarning("Selection Error", "Please select a tenant to edit.")

# Function to delete a tenant
def delete_tenant():
    try:
        selected_id = tenant_listbox.get(tenant_listbox.curselection()).split(':')[0]
        cur.execute("DELETE FROM tenant WHERE id = ?", (selected_id,))
        conn.commit()
        log_action(selected_id, 'delete')
        refresh_listbox()
    except tk.TclError:
        messagebox.showwarning("Selection Error", "Please select a tenant to delete.")

# Function to log actions
def log_action(tenant_id, action):
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    cur.execute("INSERT INTO tenant_log (tenant_id, action, timestamp) VALUES (?, ?, ?)", (tenant_id, action, timestamp))
    conn.commit()

# Function to refresh the listbox
def refresh_listbox():
    tenant_listbox.delete(0, tk.END)
    cur.execute("SELECT id, name, plate_number FROM tenant")
    for row in cur.fetchall():
        tenant_listbox.insert(tk.END, f"{row[0]}: {row[1]} - {row[2]}")

# Function to toggle full screen
def toggle_fullscreen(event=None):
    app.attributes("-fullscreen", not app.attributes("-fullscreen"))

# Function to exit full screen
def end_fullscreen(event=None):
    app.attributes("-fullscreen", False)

# Create the main application window
app = tk.Tk()
app.title("Tenant Management")

# Make the window full screen
app.attributes("-fullscreen", True)
app.bind("<F11>", toggle_fullscreen)
app.bind("<Escape>", end_fullscreen)

# Configure grid to expand with window size
app.grid_rowconfigure(0, weight=1)
app.grid_rowconfigure(1, weight=1)
app.grid_rowconfigure(2, weight=1)
app.grid_rowconfigure(3, weight=5)
app.grid_rowconfigure(4, weight=1)

app.grid_columnconfigure(0, weight=1)
app.grid_columnconfigure(1, weight=1)
app.grid_columnconfigure(2, weight=3)

# Create frames for better widget placement
input_frame = tk.Frame(app)
input_frame.grid(row=0, column=0, columnspan=4, padx=10, pady=10, sticky='nsew')

listbox_frame = tk.Frame(app)
listbox_frame.grid(row=0, column=2, rowspan=5, padx=10, pady=10, sticky='nsew')

# Create and place the widgets within frames
tk.Label(input_frame, text="Name", font=("Arial", 16)).grid(row=0, column=0, padx=10, pady=10, sticky='e')
tk.Label(input_frame, text="Plate Number", font=("Arial", 16)).grid(row=1, column=0, padx=10, pady=10, sticky='e')

name_entry = tk.Entry(input_frame, font=("Arial", 16))
plate_entry = tk.Entry(input_frame, font=("Arial", 16))

name_entry.grid(row=0, column=1, padx=10, pady=10, sticky='ew')
plate_entry.grid(row=1, column=1, padx=10, pady=10, sticky='ew')

add_button = tk.Button(input_frame, text="Add Tenant", font=("Arial", 16), command=add_tenant)
edit_button = tk.Button(input_frame, text="Edit Tenant", font=("Arial", 16), command=edit_tenant)
delete_button = tk.Button(input_frame, text="Delete Tenant", font=("Arial", 16), command=delete_tenant)

add_button.grid(row=2, column=1, padx=10, pady=10, sticky='ew')
edit_button.grid(row=3, column=1, padx=10, pady=10, sticky='ew')
delete_button.grid(row=4, column=1, padx=10, pady=10, sticky='ew')

tenant_listbox = tk.Listbox(listbox_frame, font=("Arial", 16))
tenant_listbox.pack(fill=tk.BOTH, expand=True)

refresh_listbox()

# Run the application
app.mainloop()

# Close the database connection when the application is closed
conn.close()
