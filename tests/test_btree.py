from __future__ import annotations

from math import inf
from typing import Any, Optional
import sys

from sortedcontainers import SortedList
import pytest
from loguru import logger


class BTreeNode:
    def __init__(self, order: int, is_leaf: bool) -> None:
        self.order = order
        self.is_leaf = is_leaf

        self.keys: SortedList[float] = SortedList()
        self.values: dict[float, Any] = {}
        self.children: dict[float, Optional[BTreeNode]] = {}

        self.add(inf, None, None)

    def __repr__(self):
        ls = self.repr()
        return "\n".join(ls)

    def repr(self, level: int = 0) -> list[str]:
        logger.trace(f"{self.keys=} {self.values.keys()=} {self.children.keys()=}")
        s = "    " * level + f" {level=}"
        s += " keys=(" + ", ".join([str(k) for k in self.keys]) + ")"
        s += " values=(" + ", ".join([str(self.values[k]) for k in self.keys]) + ")"
        s += " leaf" if self.is_leaf else ""
        s += " full" if len(self.keys) == 2 * self.order else ""

        result = [s]
        for key in self.keys:
            child = self.children[key]
            if child is not None:
                result += child.repr(level=level + 1)
        return result

    def is_full(self) -> bool:
        return len(self.keys) == self.order * 2

    def has(self, key: float) -> bool:
        return key in self.keys

    def add(self, key: float, value: Any, child: Optional[BTreeNode]) -> None:
        assert not self.has(key)
        self.keys.add(key)
        self.values[key] = value
        self.children[key] = child

    def update(self, key: float, value: Any, child: Optional[BTreeNode]) -> None:
        assert self.has(key)
        self.values[key] = value
        self.children[key] = child

    def pop(self, key: float) -> tuple[Any, Optional[BTreeNode]]:
        v = self.values[key]
        c = self.children[key]
        self.keys.remove(key)
        del self.values[key]
        del self.children[key]
        return (v, c)


class BTree:
    def __init__(self, order: int):
        assert order > 0, f"{order=}"
        self.order: int = order
        self.root: BTreeNode = BTreeNode(self.order, True)

    def __repr__(self) -> str:
        ls = self.root.repr(0)
        return "\n".join(ls)

    def insert(self, key: int, data: Any):
        logger.trace(f"{key=} {data=}")
        if self.root.is_full():
            # grow the root
            t = BTreeNode(self.order, False)
            t.children[inf] = self.root
            self.root = t
            # split the node
            self._split_child(self.root, inf)
            # insert again
            self._insert_not_full(self.root, key, data)
        else:
            self._insert_not_full(self.root, key, data)

    def _insert_not_full(self, node: BTreeNode, key: int, value: Any) -> None:
        if node.is_leaf:
            # for leaf node, each key is corresponding to one data entry
            # to add a new data entry, we need to add a new key
            if node.has(key):
                return node.update(key, value, None)
            return node.add(key, value, None)
        else:
            # for none-leaf node, find if the key is matching its own
            if node.has(key):
                return node.update(key, value, None)
            # if not its own, key then find the right child to insert
            # e.g. if it has 3 keys [k0, k1, k2] (k0 < k1 < k2)
            # its children's range shall be (-inf, k0), (k0, k1), (k1, k2), (k2, +inf)
            for k in node.keys:
                if key < k:
                    break
            child = node.children[k]
            assert child is not None
            if child.is_full():
                # split it first, then retry the insertion
                self._split_child(node, k)
                return self._insert_not_full(node, key, value)
            return self._insert_not_full(child, key, value)

    def _split_child(self, root: BTreeNode, key: float) -> None:
        logger.trace(f"{key=} root=\n{repr(root)}")
        assert not root.is_full()
        assert root.has(key)
        child = root.children[key]
        assert child is not None
        assert child.is_full()

        mid_index = self.order - 1
        new_child = BTreeNode(self.order, child.is_leaf)
        keys = [k for k in child.keys]
        for i, k in enumerate(keys):
            if i < mid_index:
                v, c = child.pop(k)
                # smaller keys goes to new child node
                new_child.add(k, v, c)
            elif i == mid_index:
                v, c = child.pop(k)
                # the middle child goes to new child node, but with inf key
                # the middle value goes to root
                # and insert new child to root's children with the middle key
                new_child.update(inf, None, c)
                root.add(k, v, new_child)
            else:
                # the larger keys stays
                break
        logger.trace(f"root=\n{repr(root)}")

    def search(self, key: float) -> Any:
        node = self._search(self.root, key)
        if node is not None:
            return node.values[key]
        return None

    def _search(self, node: BTreeNode, key: float) -> Optional[BTreeNode]:
        if node.has(key):
            return node
        if node.is_leaf:
            return None
        for k in node.keys:
            if key < k:
                break
        child = node.children[k]
        assert child is not None
        return self._search(child, key)


@pytest.fixture
def btree():
    return BTree(2)  # Order 2 B-tree


def test_empty_tree(btree):
    """Test that a newly created tree is empty"""
    assert btree.search(1) is None


def test_single_insert_search(btree):
    """Test inserting and searching a single value"""
    btree.insert(5, "five")
    assert btree.search(5) == "five"
    assert btree.search(6) is None


def test_multiple_inserts(btree):
    """Test inserting multiple values and searching them"""
    test_data = [(1, "one"), (2, "two"), (3, "three"), (4, "four"), (5, "five")]
    for key, value in test_data:
        btree.insert(key, value)

    for key, value in test_data:
        assert btree.search(key) == value


def test_duplicate_insert(btree):
    """Test inserting duplicate keys"""
    btree.insert(1, "one")
    btree.insert(1, "ONE")
    assert btree.search(1) == "ONE"


def test_node_splitting(btree):
    """Test that the tree correctly splits nodes when they become full"""
    # Insert enough values to cause multiple splits
    values = list(range(10))
    for value in values:
        btree.insert(value, str(value))

    # Verify all values are still accessible
    for value in values:
        assert btree.search(value) == str(value)


def test_large_values(btree):
    """Test the tree with large key values"""
    btree.insert(1000000, "large")
    btree.insert(-1000000, "negative")
    assert btree.search(1000000) == "large"
    assert btree.search(-1000000) == "negative"


def test_mixed_types(btree):
    """Test storing different types of values"""
    test_data = [
        (1, "string"),
        (2, 123),
        (3, [1, 2, 3]),
        (4, {"key": "value"}),
        (5, (1, 2, 3)),
    ]
    for key, value in test_data:
        btree.insert(key, value)

    for key, value in test_data:
        assert btree.search(key) == value


def test_different_orders():
    """Test B-trees of different orders"""
    orders = [2, 3, 4]
    for order in orders:
        tree = BTree(order)
        # Insert enough values to force multiple splits
        for i in range(order * 4):
            tree.insert(i, f"value{i}")
            assert tree.search(i) == f"value{i}"


def test_sequential_insert():
    """Test inserting values in sequential order"""
    tree = BTree(3)
    values = list(range(20))
    for val in values:
        tree.insert(val, f"val{val}")

    # Verify all values are present
    for val in values:
        assert tree.search(val) == f"val{val}"


def test_reverse_insert():
    """Test inserting values in reverse order"""
    tree = BTree(3)
    values = list(range(20))[::-1]  # Reverse order
    for val in values:
        tree.insert(val, f"val{val}")

    # Verify all values are present
    for val in values:
        assert tree.search(val) == f"val{val}"


def test_float_keys(btree):
    """Test using float keys"""
    test_data = [(1.5, "a"), (2.3, "b"), (3.7, "c")]
    for key, value in test_data:
        btree.insert(key, value)

    for key, value in test_data:
        assert btree.search(key) == value


def test_error_cases():
    """Test error cases"""
    with pytest.raises(AssertionError):
        BTree(0)  # Order must be positive

    with pytest.raises(AssertionError):
        BTree(-1)  # Order must be positive


if __name__ == "__main__":
    # Exit with the pytest's exit code
    sys.exit(pytest.main([__file__, "-s"]))
