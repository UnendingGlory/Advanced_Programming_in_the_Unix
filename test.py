import threading

n = 0


def add():
    global n
    for _ in range(1000000):
        n = n + 1


def sub():
    global n
    for _ in range(1000000):
        n = n - 1


if __name__ == "__main__":
    t1 = threading.Thread(target=add)
    t2 = threading.Thread(target=sub)

    t1.start()
    t2.start()

    t1.join()
    t2.join()

    print("n的值为:", n)
