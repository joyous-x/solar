# coding=utf-8
import os


class CategoryItem(object):
    def __init__(self, unique_id=-1):
        self.id = unique_id
        self.name = None
        self.level = 0
        self.title = None
        self.desc = None
        self.keywords = None
        self.permalink = None
        self.published = False


class Categories(object):
    def __init__(self):
        self._items = []
        self.root_node = None

    def size(self):
        return len(self.items)

    def get(self, item_id):
        if item_id not in self._items:
            return None
        return self._items[item_id]

    def put(self, item):
        self._items.append(item)


class CategoryNode(object):
    def __init__(self, categories: Categories):
        self._categories = categories
        self.parent_id = None
        self.children_ids = set()

    def add_child(self, item):
        item_id = self._categories.size()
        self.children_ids.add(item_id)
        self._categories.put(item)

    def set_parent(self, item_id):
        parent = self._categories.get(item_id)
        if parent is not None:
            self.parent_id = item_id
        return parent

    def children(self):
        items = []
        for item_id in self.children_ids:
            item = self._categories.get(item_id)
            if item is None:
                continue
            items.append(item)
        return items


class BlogCategory(object):
    """ BlogCategory
    travel through the blogs and make categories using README.md.
    the blogs structure looks like this:
        first and second level are named by the directory name,  others file name's prefix
        eg. a/b/c_xxx.md have category a/b/c
    """
    def __init__(self, blog_root):
        self.blog_root = blog_root
        self.categories = Categories()

    def travel(self):
        root_dir = self.blog_root
        category_first = os.listdir(root_dir)
        category_first = filter(lambda x: not os.path.isdir(os.path.join(root_dir, x)), category_first)
        for category in category_first:
            self._make_category_in_dirs(root_dir, category)

    def _make_category_in_dirs(self, root_dir, parent_category):
        categories = os.listdir(root_dir)
        categories = filter(lambda x: not os.path.isdir(os.path.join(root_dir, x)), categories)
