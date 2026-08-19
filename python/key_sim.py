import iio
import time
from queue import Queue, Empty
from threading import Thread, Event
from pynput import keyboard
from pynput.keyboard import Controller
import math
import tkinter as tk

# Setari de timp
GUI_UPDATE_TIME = 0.05  # 50 ms pentru update-ul interfetei

# Instantiem controller-ul care va apasa tastele in Windows
kbd = Controller()

# Mapam miscarile catre taste specifice
KEY_MAP = {
    'front': 'w',
    'back': 's',
    'left': 'a',
    'right': 'd'
}

def create_movement_gui(queue: Queue):
    root = tk.Tk()
    root.title("Monitorizare Senzor 3D")
    
    canvas = tk.Canvas(root, width=400, height=400, bg="#222")
    canvas.pack()
    
    square_size = 80
    
    # CRUCE (D-PAD)
    squares = {
        'left':  canvas.create_rectangle(50, 150, 50 + square_size, 150 + square_size, fill='white'),
        'right': canvas.create_rectangle(50 + 2*square_size, 150, 50 + 3*square_size, 150 + square_size, fill='white'),
        'front': canvas.create_rectangle(50 + square_size, 150 - square_size, 50 + 2*square_size, 150, fill='white'),
        'back':  canvas.create_rectangle(50 + square_size, 150 + square_size, 50 + 2*square_size, 150 + 2*square_size, fill='white'),
    }

    def update_square(canvas, square, on_time):
        intensity = 255 - int(on_time * 255)
        color = f'#{intensity:02x}{intensity:02x}ff'
        canvas.itemconfig(square, fill=color)

    def update_gui():
        try:
            mov = queue.get_nowait()
            update_square(canvas, squares['left'], mov['left'])
            update_square(canvas, squares['right'], mov['right'])
            update_square(canvas, squares['front'], mov['front'])
            update_square(canvas, squares['back'], mov['back'])
        except Empty:
            pass 
            
        root.after(int(GUI_UPDATE_TIME * 1000), update_gui)
        
    update_gui()
    root.mainloop()


def get_data(device):
    axes = ["x", "y", "z"]
    axis_data = {
        'x': {'+': 0, '-': 0},
        'y': {'+': 0, '-': 0},
        'z': {'+': 0, '-': 0}
    }
    lista_canale = [f"voltage{i}" for i in range(6)]
    for i, nume_canal in enumerate(lista_canale):
        ch = device.find_channel(nume_canal, False)
        if ch is None:
            raise ValueError(f"Could not find channel {nume_canal}")
            
        attr = int(ch.attrs['raw'].value)
        axis = axes[i // 2]
        polarity = '+' if i % 2 == 0 else '-'
        axis_data[axis][polarity] = attr
        
    return axis_data


def get_roll_pitch(axis: dict):
    x = axis['x']['+'] - axis['x']["-"]
    y = axis['y']['+'] - axis['y']["-"]
    z = axis['z']['+'] - axis['z']["-"]
    
    roll = math.atan2(y, z) * 180 / math.pi
    pitch = math.atan2(-x, math.sqrt(y**2 + z**2)) * 180 / math.pi
    
    return roll, pitch


def get_movement(roll, pitch):
    mov = {'left': 0.0, 'right': 0.0, 'front': 0.0, 'back': 0.0}
    if pitch > 0:
        mov['front'] = min(pitch / 45.0, 1.0)
    elif pitch < 0:
        mov['back'] = min(abs(pitch) / 45.0, 1.0)
        
    if roll > 0:
        mov['right'] = min(roll / 45.0, 1.0)
    elif roll < 0:
        mov['left'] = min(abs(roll) / 45.0, 1.0)
        
    mov = {k: round(v, 2) for k, v in mov.items()}
    return mov


def threaded_keypress(key_char, on_time):
    # Daca miscarea e sub 30%, nu apasam tasta (filtram zgomotul)
    if on_time < 0.3:
        return

    def press_task():
        kbd.press(key_char)
        # Tinem tasta apasata maxim 0.1 secunde
        time.sleep(on_time * 0.1) 
        kbd.release(key_char)

    t = Thread(target=press_task)
    t.daemon = True
    t.start()


def start_iio(device):
    data = get_data(device)
    roll, pitch = get_roll_pitch(data)
    mov = get_movement(roll, pitch)

    # Itereaza prin fiecare directie si apasa tasta doar daca placa e inclinata
    for direction, intensity in mov.items():
        if intensity > 0:  
            mapped_key = KEY_MAP.get(direction)
            if mapped_key:
                threaded_keypress(mapped_key, intensity)

    # Print in consola pentru debug
    print(f"\rRoll: {roll:5.1f}° | Pitch: {pitch:5.1f}° | Mov: {mov} \033[K", end="", flush=True)
    return mov


def threaded_function(queue: Queue):
    try:
        device = init_iio()
    except Exception as e:
        print(f"Eroare iio: {e}")
        return

    while not stop_event.is_set():
        if pause_event.is_set():
            queue.put(start_iio(device))
            # Citim datele o data la 0.1 secunde. Se potriveste perfect cu apasarea tastei.
            time.sleep(0.1)
        else:
            time.sleep(0.2)


def init_iio():
    ctx = iio.NetworkContext("10.76.84.250")
    device = ctx.find_device("ad5592r_s")
    if device is None:
        raise ValueError("Eroare: Nu am gasit device-ul ad5592r_s!")
    return device


def on_keypress(key):
    try:
        if key == keyboard.Key.ctrl_l or key == keyboard.Key.ctrl_r or key == keyboard.Key.ctrl:
            if pause_event.is_set():
                pause_event.clear()
                print("\n[PAUZĂ] Senzorul a fost oprit.")
            else:
                pause_event.set()
                print("\n[START] Senzorul citește date...")
                
        elif key == keyboard.Key.esc:
            pause_event.clear()
            stop_event.set()
            print("\n[EXIT] Ieșire din program...")
            return False 
    except AttributeError:
        pass
    return True


if __name__ == "__main__":
    pause_event = Event()
    stop_event = Event()
    
    movement = {'left': 0.0, 'right': 0.0, 'front': 0.0, 'back': 0.0}
    movement_que = Queue()
    movement_que.put(movement)

    # Firul de executie pentru senzor
    iio_thread = Thread(target=threaded_function, args=(movement_que,))
    iio_thread.daemon = True
    iio_thread.start()

    # Firul de executie pentru citirea tastaturii (CTRL, ESC)
    keyboard_listener = keyboard.Listener(on_press=on_keypress)
    keyboard_listener.daemon = True
    keyboard_listener.start()

    print("\n--- PROGRAM PORNIT ---")
    print("-> Apasa CTRL pentru a porni/pune pe pauza citirea.")
    print("-> Apasa ESC pentru a inchide scriptul (si fereastra).\n")
    
    # GUI-ul porneste in Main Thread
    create_movement_gui(movement_que)