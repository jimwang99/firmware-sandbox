#pragma once

#include <stdint.h>

#include "circular_buffer.hpp"

enum class EventId : uint16_t {
  NONE = 0,
  START_UP,
  SHUT_DOWN,
  START_READING,
  START_WRITING,
  STOP_READING,
  STOP_WRITING,
  MAX,
};

struct Event {
  EventId id : 16;
  uint64_t timestamp : 48;
};

class EventLogger {
 private:
  CircularBuffer<Event> cb_;

 public:
  EventLogger(int size) : cb_(size) {}

  bool log(EventId id) {
    // LOG_DBG("EventId=%d", static_cast<int>(id));
    LOG_DBG("EventId");
    Event ev;
    ev.id = id;
    ev.timestamp = k_cycle_get_64();
    return cb_.write(ev);
  }

  Event get() {
    Event ev;
    auto result = cb_.read(ev);
    if (!result) {
      ev.id = EventId::NONE;
    }
    return ev;
  }
};