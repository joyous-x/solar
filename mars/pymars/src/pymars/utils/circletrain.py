# coding=utf-8
import threading
import time


class MiQueue(object):
    def __init__(self):
        pass

    def pop(self):
        raise NotImplementedError("Queue.pop not implemented")

    def push(self):
        raise NotImplementedError("Queue.push not implemented")


class LimitedList(list):
    """
    LimitedList:
        a list with max length
    """
    def __init__(self, max_len: int=0):
        super(LimitedList, self).__init__([])
        self.max_len = max_len

    def append(self, item):
        """circletrain.py
        :param item
        :return: if len(LimitedList) >= max_len, return False; otherwise return True
        """
        if len(self) >= self.max_len:
            return False
        super(LimitedList, self).append(item)
        return True


class RepeatTimer(object):
    def __init__(self, interval, function, *args, **kwargs):
        self._timer = None
        self.interval = interval
        self.function = function
        self.args = args
        self.kwargs = kwargs

    def _run(self):
        self.function(*self.args, **self.kwargs)

    def start(self):
        if self._timer is None:
            self._timer = threading.Timer(self.interval, self._run)
        self._timer.start()

    def stop(self):
        if self._timer is None:
            return
        self._timer.cancel()


class CircleTrainQueue(MiQueue):
    def __init__(self, options: dict()):
        self.capacity = options["capacity"] if "capacity" in options else 9
        self.speed = options["speed"] if "speed" in options else 3
        self.carriage_capacity = options["carriage_capacity"] if "carriage_capacity" in options else 8
        self._train = [LimitedList(self.carriage_capacity) for i in range(self.capacity)]

        self.pos_push, self.pos_pop = 1, 0
        self._timer = RepeatTimer(1.0/self.speed, self._run)
        self._timer.start()

    def __str__(self):
        train = "capacity={} speed={} carriage={} pos_push={} pos_pop={}"
        train = train.format(self.capacity, self.speed, self.carriage_capacity, self.pos_push, self.pos_pop)
        return train

    def _run(self):
        """
        update the positions: push, pop
        """
        self._move_pos(push=True, pop=False)

    def _move_pos(self, push=True, pop=False):
        if pop:
            self._train[self.pos_pop].clear()
            tmp = (self.pos_pop + 1) % self.capacity
            if tmp != self.pos_push:
                self.pos_pop = tmp
            else:
                print("pop is just following push, train is empty")

        if push:
            tmp = (self.pos_push + 1) % self.capacity
            if tmp != self.pos_pop:
                self.pos_push = tmp
                self._train[self.pos_push].clear()
            else:
                print("push will catch up pop, train is full")

    def push(self, item, block=True):
        rst = False
        while True:
            if self.pos_push < 0 or self.pos_push >= self.capacity:
                break
            rst = self._train[self.pos_push].append(item)
            if not rst:
                self._move_pos(push=True)
            if rst or not block:
                break
            time.sleep(0.5)
        return rst

    def pop(self):
        if self.pos_pop < 0 or self.pos_pop >= self.capacity:
            return None
        rsts = self._train[self.pos_pop].copy()
        self._move_pos(push=False, pop=True)
        return [i for i in rsts]


if __name__ == "__main__":
    train = CircleTrainQueue({})
    for i in range(10):
        rst = train.push(i, block=True)
        print("push {} resulst={}".format(i, rst))
        time.sleep(1)
        if i % 3 == 0:
            items = train.pop()
            print("pop {}".format(items))
        print(str(train))
    for i in range(3):
        items = train.pop()
        print("pop {}, train is {}".format(items, str(train)))

