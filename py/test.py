import iio

# create venv in py windows
# in vscode terminal
# python3 -m venv venv
# ./venv/Scripts/activate
# python3 -m pip install pylibiio



def andrei(x: list) -> bool: # wrong
        x = x.copy() # retains a copy of it else it gets deleted outside
        if len(x) == 0:
                print("ciau")
        elif len(x) == 1:
                print("close enough")
        else:
                print("stau")
        

        for index, item in enumerate(range(5,15, 2)):
                print(index, item)

        while x:
                print(x, end='') 
                print("")
                x.pop()
        return True


def main():
        x= ["string, 2"]
        andrei(x) # wrong
        print(x)
        print("hello world")

        x = "my_name"
        print(type(x))
        y = 7
        x = 7
        print(type(x))
        x = 7.1
        print(type(x))

        x = [9, [3, 2.2], "salut"]
        print(type(x))
        print(x.__str__())
        print(x[1][1])

        x = {
                "hi" : "bye",
                1 : 7
        }
        print(type(x))
        print(x["hi"])
        print(x[1])

        x = (9, 0, "asdasda", x )
        print(type(x))
        print(x)

if __name__ == "__main__":
        main()




