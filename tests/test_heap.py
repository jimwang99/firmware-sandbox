from __future__ import annotations
from typing import Any, Iterable

from loguru import logger


class Heap:
    def __init__(self, keys: Iterable[Any], values: Iterable[Any], max_or_min: str = "max") -> None:
        self.keys = []
        self.values = []
        self.max_or_min = max_or_min
        self.func = max if max_or_min == "max" else min

        for k, v in zip(keys, values):
            self.add(k, v)

    def __repr__(self) -> str:
        return self.max_or_min + " " + ", ".join(str(k) for k in self.keys)
    
    def _heapify_one_node(self, index: int) -> int:
        # if changes have not been made, return 0
        # swapped root with left, return 1
        # swapped root with right, return 2
        i = index
        j = 2 * i + 1
        k = 2 * i + 2

        ls = [(i, self.keys[i])]
        if j < len(self.keys):
            ls.append((j, self.keys[j]))
        if k < len(self.keys):
            ls.append((k, self.keys[k]))
        r, _ = self.func(ls, key=lambda x: x[1])
        
        if r == i: # no swapping is needed
            return 0

        self.keys[i], self.keys[r] = self.keys[r], self.keys[i]
        self.values[i], self.values[r] = self.values[r], self.values[i]
        return 1 if r == j else 2
        
    def add(self, key: Any, value: Any) -> None:
        # add the new node to the end
        i = len(self.keys)
        self.keys.append(key)
        self.values.append(value)

        if i == 0:
            # single node, no need to heapify
            return

        # heapify stage by stage from the new node
        while True:
            # get index of its parent
            i = (i // 2 - 1) if (i % 2 == 0) else (i // 2)
            if i < 0: # out of index
                break
            # heapify node
            r = self._heapify_one_node(i)
            if r == 0: # no changes, early exit
                break
        logger.debug(repr(self))

    def pop(self) -> tuple[Any, Any]:
        # return key and value of the top node
        if len(self.keys) == 0:
            raise RuntimeError("Heap is empty")

        result_key = self.keys.pop(0)
        result_value = self.values.pop(0)

        if len(self.keys) <= 1:
            # single node, no need to heapify
            return result_key, result_value

        self.keys.insert(0, self.keys.pop(-1))
        self.values.insert(0, self.values.pop(-1))
        
        # heapify stage by stage from the root node
        i = 0
        while True:
            r = self._heapify_one_node(i)
            if r == 0: # no changes, early exit
                break
            i = i * 2 + r
            assert i < len(self.keys)

        logger.debug(repr(self))
        return result_key, result_value

    
def _test(ls: list[int], max_or_min: str):
    hp = Heap(ls, [str(x) for x in ls], max_or_min=max_or_min)
    logger.debug(repr(hp))
    
    f = max if max_or_min == "max" else min
    
    while ls:
        m = f(ls)
        ls.remove(m)

        k, _  = hp.pop()
        assert k == m
    
    
def test_basic():
    _test([3, 1, 2, 4], "max")
    _test([3, 1, 2, 4], "min")
    
def test_random():
    import random

    for _ in range(20):
        ls = [random.randint(0, 100) for _ in range(20)]
        _test(ls, "max")
        _test(ls, "min")
    

if __name__ == "__main__":
    import sys
    import pytest
    # Exit with the pytest's exit code
    sys.exit(pytest.main([__file__, "-s"]))