#pragma once
// memento.hpp — Memento design pattern with DataBuffer-backed snapshots.
//
// Inherit from Memento and implement _saveToSnapshot / _loadFromSnapshot
// as private methods(with Memento declared as friend).
// Snapshot is a DataBuffer, enabling elegant serialization reuse.

#include <vector>
#include "libcpp/data/data_buffer.hpp"

namespace libcpp {
namespace core {

/// @brief Abstract Memento base class with undo/redo history.
/// Subclass and implement _saveToSnapshot / _loadFromSnapshot.
class Memento {
 public:
  using Snapshot = data::DataBuffer;

  virtual ~Memento() = default;

  Snapshot save() const;
  void load(const Snapshot& state);

  // History management(undo/redo)
  void pushHistory();
  [[nodiscard]] bool undo();
  [[nodiscard]] bool redo();
  [[nodiscard]] size_t historySize() const;

 private:
  virtual void _saveToSnapshot(Snapshot& snapshot) const = 0;
  virtual void _loadFromSnapshot(Snapshot& snapshot) = 0;

  std::vector<Snapshot> _history;
  size_t _historyPos = 0;
};

}  // namespace core
}  // namespace libcpp
