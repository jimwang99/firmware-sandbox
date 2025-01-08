from typing import Any, Optional, TypeVar, Iterator

TK = TypeVar("TK", int, str)


class BinarySearchTreeNode:
    def __init__(self, key: TK, data: Any):
        self.key: TK = key  # type: ignore[assignment]
        self.data: Any = data
        self.parent: Optional[BinarySearchTreeNode] = None
        self.left: Optional[BinarySearchTreeNode] = None
        self.right: Optional[BinarySearchTreeNode] = None


class BinarySearchTree:
    def __init__(self) -> None:
        self.root: Optional[BinarySearchTreeNode] = None

    def insert(self, key: TK, data: Any) -> BinarySearchTreeNode:
        node = BinarySearchTreeNode(key, data)
        if self.root is None:
            self.root = node
            return node

        assert type(key) is type(self.root.key), f"{type(key)=} {type(self.root.key)=}"
        assert type(data) is type(
            self.root.data
        ), f"{type(data)=} {type(self.root.data)=}"
        self._insert(self.root, node)
        return node

    def _insert(self, root: BinarySearchTreeNode, new: BinarySearchTreeNode) -> None:
        if root.key == new.key:
            root.data = new.data
            return

        if new.key < root.key:  # type: ignore[operator]
            if root.left:
                self._insert(root.left, new)
            else:
                root.left = new
        elif new.key > root.key:  # type: ignore[operator]
            if root.right:
                self._insert(root.right, new)
            else:
                root.right = new

    def search(self, key: TK) -> Optional[BinarySearchTreeNode]:
        return self._search(self.root, key)

    def _search(
        self, root: Optional[BinarySearchTreeNode], key: TK
    ) -> Optional[BinarySearchTreeNode]:
        if root is None:
            return None
        if root.key == key:
            return root
        if key < root.key:  # type: ignore[operator]
            return self._search(root.left, key)
        return self._search(root.right, key)

    def inorder(self) -> Iterator[BinarySearchTreeNode]:
        for node in self._inorder(self.root):
            yield node

    def _inorder(self, node) -> Iterator[BinarySearchTreeNode]:
        if node is not None:
            for n in self._inorder(node.left):
                yield n
            yield node
            for n in self._inorder(node.right):
                yield n


def test_basic():
    bst = BinarySearchTree()
    bst.insert(10, None)
    assert bst.root.key == 10

    bst.insert(5, None)
    assert bst.root.left.key == 5
    assert bst.root.right is None

    bst.insert(15, None)
    assert bst.root.right.key == 15

    bst.insert(2, None)
    assert bst.root.left.left.key == 2

    keys = [n.key for n in bst.inorder()]
    assert keys == [2, 5, 10, 15]
