from __future__ import annotations

from typing import Any


class PrintTreeNode:
    def __init__(self, value: Any) -> None:
        self.value = value
        self.level = 0
        self.children: list[PrintTreeNode] = []
        self.parent: PrintTreeNode = None

    def __str__(self) -> str:
        s = "    " * (self.level - 1) if self.level > 0 else ""
        s += "+-- " if self.level > 0 else ""
        s += str(self.value)
        for n in self.children:
            s += "\n" + str(n)
        return s

    def add_child(self, node: PrintTreeNode) -> None:
        node.level = self.level + 1
        node.parent = self
        self.children.append(node)

def test_basic():
    n1 = PrintTreeNode(1)
    n2 = PrintTreeNode(2)
    n3 = PrintTreeNode(3)
    n4 = PrintTreeNode(4)
    n1.add_child(n2)
    n1.add_child(n3)
    n2.add_child(n4)
    print(n1)

if __name__ == "__main__":
    # Exit with the pytest's exit code
    import sys
    import pytest
    sys.exit(pytest.main([__file__, "-s"]))