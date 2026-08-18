import iio
from queue import Queue
from threading import Event, Thread

def create_movement_gui(queue: Queue):
    pass

def get_data(device):
    axis_data = { 
        'x': {'-': int, '+': int}, 
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int},
    }

    axes = ['x', 'y', 'z']

    channel_names = [f'voltage{i}' for i in range(6)]
    for i, channel_name in enumerate(channel_names):
        ch = device.find_channel(channel_name)
        if ch is None:
            raise ValueError("could not find channel")
        
        attr = ch.attrs['raw'].value

        axis = axes[i // 2]
        polarity = '+' if i % 2 == 0 else '-'
        axis_data[axis][polarity] = attr

    return axis_data

def start_iio(device):
    data = get_data(device)
    print(data)
    # roll, pitch = get_roll_pitch(data)
    # movement = get_movement(roll, pitch)
    #return movement 

def threaded_function(queue: Queue):
    device = init_iio()

    #while True:
    queue.put(start_iio(device))

def init_iio():
    ctx = iio.Context("ip:10.76.84.113")
    dev = ctx.find_device("ad5592r_s")
    if dev is None:
        raise ValueError("failed to get device")

    return dev

if __name__ == "__main__":
    pause_event = Event()
    start_event = Event()

    movement = {'left': 0, 'right': 0, 'back': 0, 'front': 0}
    movement_queue = Queue()
    movement_queue.put(movement)

    thread = Thread(target=create_movement_gui, args=(movement_queue, ))
    thread.daemon = True
    thread.start()

    thread = Thread(target=threaded_function, args=(movement_queue, ))
    thread.daemon = True
    thread.start()

    thread.join()