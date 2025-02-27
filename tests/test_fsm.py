
from enum import Enum, auto

class State(Enum):
    INIT = auto()
    RUNNING = auto()
    READY = auto()
    WAITING = auto()
    SUSPENDED = auto()

class ThreadStateMachine:
    def __init__(self) -> None:
        self.state = State.INIT
    
    def _check_state(self, trigger: str, possible_states: list[State]):
        if self.state not in possible_states:
            raise RuntimeError(f"{trigger=} {self.state.name=} {possible_states=}")
        
    def action(self, trigger: str) -> None:
        match trigger:
            case "start":
                self._check_state(trigger, [State.INIT, State.SUSPENDED])
                self.state = State.READY
            case "schedule":
                self._check_state(trigger, [State.READY])
                self.state = State.RUNNING
            case "wait":
                self._check_state(trigger, [State.RUNNING])
                self.state = State.WAITING
            case "suspend":
                self._check_state(trigger, [State.RUNNING, State.READY, State.INIT, State.WAITING])
                self.state = State.SUSPENDED
            case _:
                raise RuntimeError(f"Unsupported trigger={trigger}")