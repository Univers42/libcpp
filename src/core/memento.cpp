// memento.cpp — Memento pattern implementation.

#include "libcpp/core/memento.hpp"

namespace libcpp {
namespace core {

Memento::Snapshot Memento::save() const {
  Snapshot snap;
  _saveToSnapshot(snap);
  return snap;
}

void Memento::load(const Snapshot& state) {
  Snapshot copy = state;
  _loadFromSnapshot(copy);
}

void Memento::pushHistory() {
  Snapshot snap = save();
  // Erase any future states if we're not at the end
  if (_historyPos < _history.size())
    _history.erase(_history.begin() + static_cast<long>(_historyPos),
                   _history.end());
  _history.push_back(snap);
  _historyPos = _history.size();
}

bool Memento::undo() {
  if (_historyPos <= 1) return false;
  --_historyPos;
  load(_history[_historyPos - 1]);
  return true;
}

bool Memento::redo() {
  if (_historyPos >= _history.size()) return false;
  load(_history[_historyPos]);
  ++_historyPos;
  return true;
}

size_t Memento::historySize() const { return _history.size(); }

}  // namespace core
}  // namespace libcpp
