import pprint
import pytest

from loguru import logger


class BuddyAllocator:
    def __init__(self, capacity: int, degree: int) -> None:
        # make sure the capacity is 2^n
        assert (
            capacity & (capacity - 1) == 0
        ), f"Capacity must be 2^n, instead {capacity=}"
        self.C = capacity
        self.N = capacity.bit_length() - 1
        assert degree <= self.N, f"{degree=}"
        self.D = degree

        # a data structure to hold current free blocks of different size
        self.free_blocks: dict[int, list[int]] = {}
        for d in range(self.N, self.N - self.D - 1, -1):
            self.free_blocks[d] = []
        self.free_blocks[self.N].append(0)

        # a data structure that maps returned address to block's degree and index
        self.used_blocks: dict[int, tuple[int, int]] = {}

    def __repr__(self):
        return f"capacity={self.C} degree={self.D}, max-degree={self.N} free-blocks={pprint.pformat(self.free_blocks)} used-blocks={pprint.pformat(self.used_blocks)}"

    def alloc(self, size: int) -> int:
        assert size < self.C, f"{size=} {self.C=}"
        assert size > 0, f"{size=}"

        d = size.bit_length() - 1
        d = (
            self.N - self.D if d < self.N - self.D else d
        )  # requested size is smaller than min size

        # memory is available
        if len(self.free_blocks[d]) > 0:
            i = self.free_blocks[d].pop(0)
            a = self._get_address(d, i)
            assert a not in self.used_blocks.keys()
            self.used_blocks[a] = (d, i)
            return a

        # find the next best fit block
        nd = d + 1
        while nd <= self.N and len(self.free_blocks[nd]) == 0:
            nd += 1
        if nd > self.N:
            raise RuntimeError(
                f"Out of memory. requested-degree={d} free-blocks={pprint.pformat(self.free_blocks)}"
            )
        # split the larger block into the right size
        self._split(nd, d)

        i = self.free_blocks[d].pop(0)
        a = self._get_address(d, i)
        assert a not in self.used_blocks.keys()
        self.used_blocks[a] = (d, i)
        return a

    def _get_address(self, degree: int, index: int) -> int:
        return index * (2**degree)

    def free(self, address: int) -> None:
        assert address in self.used_blocks.keys(), f"{address=} {repr(self)}"
        degree, index = self.used_blocks[address]
        del self.used_blocks[address]

        self.free_blocks[degree].append(index)
        self._merge(degree, index)

    def _split(self, large_block_degree: int, requested_degree: int) -> None:
        assert (
            large_block_degree > requested_degree
        ), f"{large_block_degree=} {requested_degree=}"
        assert self.free_blocks[
            large_block_degree
        ], f"{large_block_degree=} {repr(self)}"
        index = self.free_blocks[large_block_degree].pop(0)

        # split in half
        self.free_blocks[large_block_degree - 1].append(index * 2)
        self.free_blocks[large_block_degree - 1].append(index * 2 + 1)

        if (large_block_degree - 1) == requested_degree:
            return

        # recursively split the block
        self._split(large_block_degree - 1, requested_degree)

    def _merge(self, degree: int, index: int) -> None:
        buddy = index + 1 if index % 2 == 0 else index - 1
        if index in self.free_blocks[degree] and buddy in self.free_blocks[degree]:
            self.free_blocks[degree].remove(index)
            self.free_blocks[degree].remove(buddy)
            degree += 1
            index = (index >> 1) << 1
            self.free_blocks[degree].append(index)
            self._merge(degree, index)


def test_basic_alloc():
    ba = BuddyAllocator(1024, 3)
    logger.debug(repr(ba))
    assert len(ba.free_blocks) == 4
    assert ba.free_blocks[10] == [0]
    assert ba.free_blocks[9] == []
    assert ba.free_blocks[8] == []
    assert ba.free_blocks[7] == []

    a = ba.alloc(512)
    assert a == 0
    logger.debug(repr(ba))

    a = ba.alloc(256)
    assert a == 512
    logger.debug(repr(ba))

    a = ba.alloc(128)
    assert a == 512 + 256
    logger.debug(repr(ba))

    a = ba.alloc(128)
    assert a == 512 + 256 + 128
    logger.debug(repr(ba))

    with pytest.raises(RuntimeError, match=r"Out of memory.*"):
        a = ba.alloc(128)


def test_basic_free():
    ba = BuddyAllocator(1024, 3)
    a0 = ba.alloc(128)
    a1 = ba.alloc(128)
    logger.debug(repr(ba))
    assert ba.free_blocks[10] == []
    assert ba.free_blocks[9] == [1]
    assert ba.free_blocks[8] == [1]
    assert ba.free_blocks[7] == []

    ba.free(a0)
    logger.debug(repr(ba))
    ba.free(a1)
    logger.debug(repr(ba))
    assert ba.free_blocks[10] == [0]
    assert ba.free_blocks[9] == []
    assert ba.free_blocks[8] == []
    assert ba.free_blocks[7] == []
