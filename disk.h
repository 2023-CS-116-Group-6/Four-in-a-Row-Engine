#ifndef DISK_H
#define DISK_H

#include <iostream>
using namespace std;
enum DISK_TYPE { X = 0, O = 1, EMPTY };

// interface to DISK_TYPE
class Disk {
public:
  DISK_TYPE type = EMPTY;
  Disk() {}
  Disk(DISK_TYPE type) : type(type) {}

  bool operator==(const Disk &other) const { return type == other.type; }

  bool operator!=(const Disk &other) const { return type != other.type; }

  bool operator==(const DISK_TYPE &type) const { return this->type == type; }

  bool operator!=(const DISK_TYPE &type) const { return this->type != type; }

  Disk &operator=(DISK_TYPE type) {
    this->type = type;
    return *this;
  }

  operator char() {
    constexpr char DISK_CHARS[]{'X', 'O', ' '};
    return DISK_CHARS[type];
  }

  // alternates between X and O
  void alternate() { type = DISK_TYPE(1 - type); }

  // gives O if X, X if O
  Disk counterpart() {
    Disk counter = *this;
    counter.alternate();
    return counter;
  }
};

ostream &operator<<(ostream &out, Disk disk) { return out << (char)disk; }

istream &operator>>(istream &in, Disk &disk) {
  char c;
  in >> c;
  switch (c) {
  case 'X':
    disk = X;
    break;
  case 'O':
    disk = O;
    break;
  }
  return in;
}


#endif /* DISK_H */
