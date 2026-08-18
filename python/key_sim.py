import iio
from queue import Queue
from threading import Thread

def create_movement_gui(queue: Queue):
    while True:
        if not queue.empty():
            movement = queue.get()
            
def get_data(device):
    axes=["x","y","z"]
    axis_data = {
        'x': {'+': int, '-': int},
        'y': {'+': int, '-': int},
        'z': {'+': int, '-': int}
    }
    channel_names = [f"voltage{i}" for i in range(6)]
    for i, channel_names in enumerate(channel_names):
        ch = device.find_channel(channel_names)
        if ch is None:
             raise ValueError("Could not find channel")
        attr = ch.attrs['raw'].value
        axis = axes[i//2]
        polarity='+' if i % 2 == 0 else '-'
        axis_data[axis][polarity] = attr
    return axis_data

def start_iio(device):
    data = get_data(device)
    print(data)
def threaded_function(queue: Queue):
    device = init_iio()
    queue.put(start_iio(device))

def init_iio():
        ctx = iio.Context("ip:10.76.84.250");
        device = ctx.find_device("ad5592r_s")
        if device is None:
                raise ValueError("Eroare: Nu am gasit device-ul ad5592r_s!")
        return device

if __name__=="__main__":
        movement = {'left':0, 'right':0, 'front':0, 'back':0}
        movement_que= Queue()
        movement_que.put(movement)

        thread = Thread(target=create_movement_gui, args=(movement_que,))
        thread.daemon= True;
        thread.start()

        thread = Thread(target=threaded_function, args=(movement_que,))
        thread.daemon= True;
        thread.start()

        thread.join()