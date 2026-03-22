/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_net.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 18:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_net.cpp — Chat protocol message serialization
**
** Scenario: Design a simple chat protocol with Message + MessageConsumer.
** Define message types (LOGIN, CHAT, PING, LOGOUT), serialize/deserialize
** them, and route through a dispatcher. No actual sockets — purely the
** message framing and dispatch layer.
**
** Theme: Cyber Blue (electric blue on dark slate)
*/

#include <iostream>
#include <sstream>

#include "libcpp/data/data_buffer.hpp"
#include "libcpp/net/message.hpp"
#include "libcpp/str/format.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

using libcpp::Srgb;
using libcpp::StyleSheet;
using libcpp::TermStyle;
using libcpp::TermWriter;

/* ── Cyber Blue theme ────────────────────────────────────────────────── */

LIBCPP_THEME_BEGIN(cyber_blue)
  RULE("h1", fg("#00B8D4").bg("#001520").border("#00B8D4")
                 .bold().center().pad(3, 3, 1).spacing(1, 1)
                 .glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90"))
  RULE("h2", fg("#0288D1").border("#00202E")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))
  RULE("h3", fg("#4FC3F7").border("#00202E")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c"))
  RULE("body",    fg("#90CAF9").pad_left(2))
  RULE("quote",   fg("#5C7A8A").italic().glyph("\xe2\x94\x82 ").margin_left(3))
  RULE("info",    fg("#00B8D4").glyph("\xe2\x84\xb9  "))
  RULE("warn",    fg("#FFB300").bold().glyph("\xe2\x9a\xa0  "))
  RULE("error",   fg("#EF5350").bold().glyph("\xe2\x9c\x97 "))
  RULE("success", fg("#66BB6A").glyph("\xe2\x9c\x94 "))
  RULE("trace",   fg("#4A6A7A").glyph("   "))
  RULE("sep",     border("#00202E").sep("\xe2\x94\x80"))
  RULE("bullet",  fg("#90CAF9").glyph("\xe2\x96\xaa ").pad_left(4))
  RULE("ol",      fg("#90CAF9").pad_left(4))
  CALLOUT_RULE("tip",  fg("#B3E5FC").bg("#001520").border("#0288D1")
                           .glyph("\xe2\x9c\x94 "))
  CALLOUT_RULE("note", fg("#81D4FA").bg("#001018").border("#00B8D4")
                           .glyph("\xe2\x84\xb9  "))
  TABLE_RULE(border_preset(libcpp::BORDER_ROUNDED).border_color("#0288D1")
                 .header_fg("#00B8D4").header_bg("#001520").header_font(libcpp::FONT_BOLD)
                 .header_has_bg(true)
                 .cell_fg("#90CAF9").cell_bg("#001520").cell_alt_bg("#001A28")
                 .zebra(true).cell_has_bg(true)
                 .title_fg("#00B8D4").title_bg("#001520").title_has_bg(true)
                 .footer_fg("#5C7A8A"))
  WIDTH(74)
LIBCPP_THEME_END

/* ── Protocol message types ──────────────────────────────────────────── */
enum MsgType : uint64_t {
  MSG_LOGIN  = 1,
  MSG_CHAT   = 2,
  MSG_PING   = 3,
  MSG_PONG   = 4,
  MSG_LOGOUT = 5
};

/* Global writer pointer for handler output (demo convenience) */
static TermWriter* g_w = 0;

int main() {
  TermStyle ts;
  cyber_blue().apply(ts);
  TermWriter w(ts);
  g_w = &w;

  w << "# Chat Protocol — Message Serialization";
  w << "";
  w << "Designing a chat protocol with `Message` + `MessageConsumer`.";
  w << "No sockets — purely the framing and dispatch layer.";

  // ── Protocol definition ───────────────────────────────────────────
  w << "## Protocol Definition";

  w << "| Type ID | Name | Payload |";
  w << "| :---: | :--- | :--- |";
  w << "| 1 | LOGIN | username (string) |";
  w << "| 2 | CHAT | sender (string) + text (string) |";
  w << "| 3 | PING | timestamp (int64) |";
  w << "| 4 | PONG | timestamp (int64) |";
  w << "| 5 | LOGOUT | username (string) |";
  w << "";

  // ── Build messages ────────────────────────────────────────────────
  w << "## Building Messages";

  /* LOGIN message */
  libcpp::net::Message login(MSG_LOGIN);
  login.buffer() << std::string("alice_42");
  w << "### LOGIN";
  w << "!i Payload: username = \"alice_42\"";

  /* CHAT message */
  libcpp::net::Message chat(MSG_CHAT);
  chat.buffer() << std::string("alice_42") << std::string("Hello everyone!");
  w << "### CHAT";
  w << "!i Payload: sender = \"alice_42\", text = \"Hello everyone!\"";

  /* PING message */
  libcpp::net::Message ping(MSG_PING);
  int64_t ts_now = 1711123200;
  ping.buffer() << ts_now;
  w << "### PING";
  w << ("!i Payload: timestamp = "
      + libcpp::str::to_string(static_cast<long>(ts_now)));
  w << "";

  // ── Serialization round-trip ──────────────────────────────────────
  w << "## Serialization Round-Trip";

  /* Serialize the login message */
  std::vector<uint8_t> wire = login.serialize();
  w << ("!i LOGIN serialized to "
      + libcpp::str::to_string(static_cast<int>(wire.size())) + " bytes");

  /* Deserialize it back */
  libcpp::net::Message decoded = libcpp::net::Message::deserialize(wire);
  decoded.buffer().resetCursor();
  std::string username;
  decoded.buffer() >> username;

  w << ("!v Deserialized: type="
      + libcpp::str::to_string(static_cast<long>(decoded.type()))
      + ", username=\"" + username + "\"");
  w << "";

  /* Serialize the chat message */
  std::vector<uint8_t> chat_wire = chat.serialize();
  libcpp::net::Message chat_dec =
      libcpp::net::Message::deserialize(chat_wire);
  chat_dec.buffer().resetCursor();
  std::string sender, text;
  chat_dec.buffer() >> sender >> text;

  w << ("!i CHAT serialized to "
      + libcpp::str::to_string(static_cast<int>(chat_wire.size()))
      + " bytes");
  w << ("!v Decoded: sender=\"" + sender + "\", text=\"" + text + "\"");
  w << "";

  // ── Wire format table ─────────────────────────────────────────────
  w << "### Wire Format Breakdown (LOGIN)";

  w << "| Offset | Bytes | Field |";
  w << "| ---: | :---: | :--- |";
  w << "| 0 | 8 | Message type (uint64) |";
  w << "| 8 | 8 | Payload size (uint64) |";
  w << "| 16 | 4 | String length prefix |";
  w << ("| 20 | " + libcpp::str::to_string(static_cast<int>(username.size()))
      + " | String data (\"" + username + "\") |");
  w << "";

  // ── MessageConsumer — dispatch ────────────────────────────────────
  w << "## MessageConsumer — Dispatch";
  w << "Register handlers for each message type, then dispatch:";
  w << "";

  libcpp::net::MessageConsumer consumer;
  int handled_count = 0;

  consumer.registerHandler(MSG_LOGIN,
      [&](libcpp::net::Message& m) {
        m.buffer().resetCursor();
        std::string u;
        m.buffer() >> u;
        *g_w << ("!v [LOGIN] User joined: " + u);
        ++handled_count;
      });

  consumer.registerHandler(MSG_CHAT,
      [&](libcpp::net::Message& m) {
        m.buffer().resetCursor();
        std::string s, t;
        m.buffer() >> s >> t;
        *g_w << ("!i [CHAT] " + s + ": " + t);
        ++handled_count;
      });

  consumer.registerHandler(MSG_PING,
      [&](libcpp::net::Message& m) {
        m.buffer().resetCursor();
        int64_t t;
        m.buffer() >> t;
        *g_w << ("!t [PING] timestamp = "
            + libcpp::str::to_string(static_cast<long>(t)));
        ++handled_count;
      });

  consumer.registerHandler(MSG_LOGOUT,
      [&](libcpp::net::Message& m) {
        m.buffer().resetCursor();
        std::string u;
        m.buffer() >> u;
        *g_w << ("!! [LOGOUT] User left: " + u);
        ++handled_count;
      });

  /* Dispatch all messages */
  consumer.consume(login);
  consumer.consume(chat);
  consumer.consume(ping);

  libcpp::net::Message logout(MSG_LOGOUT);
  logout.buffer() << std::string("alice_42");
  consumer.consume(logout);
  w << "";

  w << ("!v " + libcpp::str::to_string(handled_count)
      + " messages dispatched through consumer");
  w << "";

  // ── Summary callout ───────────────────────────────────────────────
  w << ">![tip] Net Module Summary";
  w << "> `Message` — typed message with DataBuffer payload";
  w << "> `serialize()` / `deserialize()` — wire-format encoding";
  w << "> `MessageConsumer` — type-to-callback dispatcher";
  w << "> `DataBuffer` — polymorphic byte container for payloads";
  w << "";

  w << "---";
  w << "!v Protocol demo complete — 4 messages sent and dispatched.";
  return 0;
}
