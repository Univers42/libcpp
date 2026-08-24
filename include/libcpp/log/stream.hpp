/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stream.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/24 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_LOG_STREAM_HPP
#define LIBCPP_LOG_STREAM_HPP

#include <sstream>
#include <string>

namespace libcpp {
namespace log {

/* ── BasicStream — a log line you build with << and never pay for if it is
**    switched off ─────────────────────────────────────────────────────────
**
** Returned by value from a factory function, streamed into, and flushed to
** its emit callback by its own destructor at the end of the full expression:
**
**     Stream debug() { return Stream(&sink, 'd', level >= DEBUG); }
**     ...
**     debug() << "fd " << fd << " sent " << n << " bytes";
**
** The point is the `enabled` flag.  When it is false NO ostringstream is
** allocated, and every operator<< is a null-pointer test the optimiser can
** see through — so a suppressed trace in a hot path costs a branch, not a
** heap allocation and a format.  A macro that builds the string first and
** filters afterwards pays in full for lines nobody reads, which at protocol
** line rate is the difference that matters.
**
** Tag is whatever the sink needs to route the finished line — a severity
** enum, or a char, or a small struct.  It is a template parameter rather
** than a fixed Level because "severity" is not the only axis a real sink
** dispatches on: a renderer may also need to know that this line is a
** banner or a success notice, which are presentation intents that no
** ordering of severities can express.
**
** Emit is a plain function pointer, not std::function: this header is part
** of the C++98-clean set.  Route through a free function or a static member
** that forwards to whatever sink object is current.
**
** COPY SEMANTICS ARE A MOVE.  C++98 has no move constructor, and returning
** by value from a factory requires an accessible copy constructor, so the
** copy transfers the buffer and leaves the source empty — which is why
** _out is mutable.  The consequence: a BasicStream must be used as a
** temporary within one full expression.  Storing one in a named variable
** and copying it works, but only the last copy emits.
*/
template <class Tag>
class BasicStream {
 public:
  typedef void (*Emit)(Tag tag, const std::string& msg);

  BasicStream(Emit emit, Tag tag, bool enabled)
      : _out((enabled && emit != 0) ? new std::ostringstream() : 0),
        _emit(emit),
        _tag(tag) {}

  BasicStream(const BasicStream& other)
      : _out(other._out), _emit(other._emit), _tag(other._tag) {
    other._out = 0;
  }

  ~BasicStream() {
    if (_out == 0) return;
    /* Detach before emitting so the buffer is freed even if the sink
    ** throws, and so a re-entrant sink cannot see a half-dead stream. */
    std::ostringstream* buf = _out;
    _out = 0;
    const std::string text = buf->str();
    delete buf;
    _emit(_tag, text);
  }

  template <class T>
  BasicStream& operator<<(const T& value) {
    if (_out != 0) *_out << value;
    return *this;
  }

  /* True when this stream will emit — useful to skip building an expensive
  ** argument that operator<< would otherwise discard. */
  bool enabled() const { return _out != 0; }

 private:
  BasicStream(); /* a stream with no destination is never useful */
  BasicStream& operator=(const BasicStream& other);

  mutable std::ostringstream* _out;
  Emit _emit;
  Tag _tag;
};

} /* namespace log */
} /* namespace libcpp */

#endif /* LIBCPP_LOG_STREAM_HPP */
