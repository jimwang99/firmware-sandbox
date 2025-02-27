from enum import IntEnum, auto
from dataclasses import dataclass
from sortedcontainers import SortedKeyList
from typing import Optional

from loguru import logger


# define command
class Command(IntEnum):
    EXECUTE_MATMUL = auto()
    ENABLE_DEBUG = auto()
    DISABLE_DEBUG = auto()
    ENABLE_PROFILING = auto()
    DISABLE_PROFILING = auto()
    MAX = auto()


# define request
@dataclass
class Request:
    priority: int
    command: Command
    args: list[int]
    invoker_id: int = 0


# define request queue
class RequestQueue:
    def __init__(self) -> None:
        self.queue: SortedKeyList[Request] = SortedKeyList(key=lambda x: x.priority)

    def add(self, request: Request) -> None:
        self.queue.add(request)

    def sched(self) -> Optional[Request]:
        if self.queue:
            return self.queue.pop(0)
        return None


# define sender
class Sender:
    def __init__(self, id: int, request_queue: RequestQueue) -> None:
        self.id = id
        self.request_queue = request_queue

    def send_request(self, request: Request) -> None:
        logger.debug(f"Invoker {self.id} sending request {request}")
        self.request_queue.add(request)


# define receiver
class Receiver:
    def __init__(self, id: int, request_queue: RequestQueue) -> None:
        self.id = id
        self.request_queue = request_queue

    def _execute_matmul(self, args: list[int]) -> None:
        pass

    def _enable_debug(self) -> None:
        pass

    def _disable_debug(self) -> None:
        pass

    def _enable_profiling(self) -> None:
        pass

    def _disable_profiling(self) -> None:
        pass

    def execute_request(self) -> None:
        request = self.request_queue.sched()
        if request:
            logger.debug(f"Client {self.id} executing request {request}")

            match request.command:
                case Command.EXECUTE_MATMUL:
                    self._execute_matmul(request.args)
                case Command.ENABLE_DEBUG:
                    self._enable_debug()
                case Command.DISABLE_DEBUG:
                    self._disable_debug()
                case Command.ENABLE_PROFILING:
                    self._enable_profiling()
                case Command.DISABLE_PROFILING:
                    self._disable_profiling()
                case _:
                    return


def test_basic():
    q = RequestQueue()
    tx0 = Sender(10, q)
    tx1 = Sender(11, q)
    rx0 = Receiver(20, q)
    rx1 = Receiver(21, q)

    r = Request()
    tx0.send_request()


if __name__ == "__main__":
    test_basic()
