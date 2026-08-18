import iio
from queue import Queue
from threading import Thread, Event

def create_movement_gui(queue : Queue):
        pass

def get_data(device):

        axis_data = {
                'x' : {'-' : int, '+' : int},
                'y' : {'-' : int, '+' : int},
                'z' : {'-' : int, '+' : int}
        }

        axes = ['x', 'y', 'z']

        channel_names = [f'voltage{i}' for i in range(6)]
        for i, channel_name in enumerate(channel_names):
                ch = device.find_channel(channel_name)
                if ch == None:
                        raise ValueError("colud not find channel")
                attr = ch.attrs['raw'].value

                polarity = '+' if i % 2 == 0 else '-'
                axis_data[axes[i // 2]][polarity] = attr

        return axis_data


def start_iio(device):
        data = get_data(device)
        print(data)
        # roll, pitch = get_roll_pitch(data)
        # movement = get_movement(roll, pitch)

        # return movement

def thread_function(queue : Queue):
        device = init_iio()

        # while True:
        queue.put(start_iio(device))         

def init_iio():
        ctx = iio.Context("ip:10.76.84.210")
        if ctx == None:
                print("Error create context")
                return None
        device = ctx.find_device("iio_adc_placa")
        if device == None:
                print("Error create device")
                return None        

        return device

if __name__ == "__main__":
        movement = {
                'left' : 0,
                'right' : 0,
                'front' : 0,
                'back' : 0
        }
        #Make a queue for shared the pointer in function 
        #not a copy of our movement dictionary
        movement_queue = Queue()
        movement_queue.put(movement)

        thread = Thread(target=create_movement_gui, args=(movement_queue, ))

        thread.daemon = True
        thread.start()

        
        thread = Thread(target=thread_function, args=(movement_queue, ))

        thread.daemon = True
        thread.start()

        thread.join()





