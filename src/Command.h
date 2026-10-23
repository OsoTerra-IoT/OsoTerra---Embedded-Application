#ifndef COMMAND_H
#define COMMAND_H

// ModestIoT: an order given to an actuator.
struct Command {
  int id;
  explicit Command(int id = 0) : id(id) {}
  bool operator==(const Command &other) const { return id == other.id; }
};

#endif
