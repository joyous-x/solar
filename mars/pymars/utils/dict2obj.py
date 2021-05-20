# coding=utf-8


class DictObject(dict):
    """ convert dict to object
    we can treat dict's keys as attributes
    """
    def __getattr__(self, key):
        return self.get(key)

    def __setattr__(self, key, value):
        self[key] = value