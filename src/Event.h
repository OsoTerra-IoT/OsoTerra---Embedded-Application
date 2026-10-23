#ifndef EVENT_H
#define EVENT_H

// ModestIoT: something that happened in a sensor.
struct Event {
  int id;
  explicit Event(int id = 0) : id(id) {}
  bool operator==(const Event &other) const { return id == other.id; }
};

#endif
