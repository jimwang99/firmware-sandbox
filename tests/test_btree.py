from math import inf
from typing import Any, Optional

from sortedcontainers import SortedList


class BTreeNode:
    def __init__(self, order: int) -> None:
        self.order = order
        self.keys: SortedList[int] = SortedList()
        self.data: list[Any] = []
        self.children: list[BTreeNode] = []

    def is_full(self) -> bool:
        return len(self.keys) == 2 * self.order - 1

    def is_leaf(self) -> bool:
        return len(self.children) == 0
    
    def add_key(self, key: int):
        pass
    
    def find_child(self, key: int):
        pass
    
    def add_data_to_leaf(self, key: int, data: Any):
        pass

class BTree:
    def __init__(self, order: int):
        assert order > 0, f"{order=}"
        self.order: int = order
        self.root: BTreeNode = BTreeNode()

    def _split_child(self, root: BTreeNode, child_index: int) -> None:
        # TODO
        child = root.children[index]
        lower = 
        if self._is_node_leaf(child):
            n = len(child.keys)
            new = BTreeNode()
            new.keys.extend(child.keys[])

    def _insert_not_full(self, node: BTreeNode, key: int, data: Any) -> None:
        assert not self._is_node_full(node)

        if self._is_node_leaf(node):
            # for leaf node, each key is corresponding to one data entry
            assert len(node.keys) == len(node.data)
            # to add a new data entry, we need to add a new key
            node.keys.add(key)
            index = node.keys.index(key)
            node.data.insert(index, data)
        else:
            # not leaf node, find the right subtree
            # for none-leaf node, number of children equals num of keys plus 1
            # e.g. if it has 3 keys [k0, k1, k2] (k0 < k1 < k2)
            # its children's range shall be (-inf, k0), (k0, k1), (k1, k2), (k2, +inf)
            assert len(node.keys) == len(
                node.data
            ), f"{len(node.keys)=} {len(node.data)=}"
            assert len(node.keys) + 1 == len(
                node.children
            ), f"{len(node.keys)=} {len(node.children)=}"

            found = False
            for i, k in enumerate(node.keys):
                if key == k:
                    node.data[i] = data
                    return
                if key < k:
                    found = True
                    break

            if not found:
                i = len(node.keys)  # choose the last one

            child = node.children[i]
            if self._is_node_full(child):
                self._split_child(node, i)
                self._insert_not_full(node, key, data)
                return

            self._insert_not_full(child, key, data)

    def insert(self, key: int, data: Any):
        if self._is_node_full(self.root):
            # grow the root
            t = BTreeNode()
            t.children.append(self.root)
            self.root = t
            # split the full node (previously root)
            self._split_child(self.root, 0)
            # insert again
            self.insert(key, data)
        else:
            self._insert_not_full(self.root, key, data)

    # Print the tree
    def print_tree(self, x, l=0):
        print("Level ", l, " ", len(x.keys), end=":")
        for i in x.keys:
            print(i, end=" ")
        print()
        l += 1
        if len(x.child) > 0:
            for i in x.child:
                self.print_tree(i, l)

    # Search key in the tree
    def search_key(self, k, x=None):
        if x is not None:
            i = 0
            while i < len(x.keys) and k > x.keys[i][0]:
                i += 1
            if i < len(x.keys) and k == x.keys[i][0]:
                return (x, i)
            elif x.leaf:
                return None
            else:
                return self.search_key(k, x.child[i])

        else:
            return self.search_key(k, self.root)


def main():
    B = BTree(3)

    for i in range(10):
        B.insert((i, 2 * i))

    B.print_tree(B.root)

    if B.search_key(8) is not None:
        print("\nFound")
    else:
        print("\nNot Found")


if __name__ == "__main__":
    main()
