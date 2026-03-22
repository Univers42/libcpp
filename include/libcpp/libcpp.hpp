/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libcpp.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 23:01:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_LIBCPP_HPP
#define LIBCPP_LIBCPP_HPP

/*
** Master include — pulls in every module of libcpp.
** Users can `#include "libcpp/libcpp.hpp"` for convenience,
** or include individual module headers for finer control.
*/

/* ── core(design patterns + fundamentals) ─────────────────────────────── */
#include "libcpp/core/arena.hpp"
#include "libcpp/core/memento.hpp"
#include "libcpp/core/observable_value.hpp"
#include "libcpp/core/observer.hpp"
#include "libcpp/core/option.hpp"
#include "libcpp/core/property.hpp"
#include "libcpp/core/result.hpp"
#include "libcpp/core/signal.hpp"
#include "libcpp/core/singleton.hpp"
#include "libcpp/core/state_machine.hpp"
#include "libcpp/core/types.hpp"

/* ── term ──────────────────────────────────────────────────────────────── */
#include "libcpp/term/color.hpp"
#include "libcpp/term/progress.hpp"
#include "libcpp/term/style.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/table.hpp"
#include "libcpp/term/thread_safe_iostream.hpp"
#include "libcpp/term/tree.hpp"
#include "libcpp/term/writer.hpp"

/* ── log ───────────────────────────────────────────────────────────────── */
#include "libcpp/log/logger.hpp"
#include "libcpp/log/macros.hpp"

/* ── test ──────────────────────────────────────────────────────────────── */
#include "libcpp/test/fuzzer.hpp"
#include "libcpp/test/snapshot.hpp"
#include "libcpp/test/spy.hpp"
#include "libcpp/test/suite.hpp"

/* ── bench ─────────────────────────────────────────────────────────────── */
#include "libcpp/bench/benchmark.hpp"
#include "libcpp/bench/profiler.hpp"
#include "libcpp/bench/timer.hpp"

/* ── mem ───────────────────────────────────────────────────────────────── */
#include "libcpp/mem/leak_guard.hpp"
#include "libcpp/mem/pool.hpp"

/* ── str ───────────────────────────────────────────────────────────────── */
#include "libcpp/str/case.hpp"
#include "libcpp/str/format.hpp"
#include "libcpp/str/utf8.hpp"

/* ── util ──────────────────────────────────────────────────────────────── */
#include "libcpp/util/argparser.hpp"
#include "libcpp/util/chronometer.hpp"
#include "libcpp/util/config.hpp"
#include "libcpp/util/timer.hpp"

/* ── data ──────────────────────────────────────────────────────────────── */
#include "libcpp/data/csv.hpp"
#include "libcpp/data/data_buffer.hpp"
#include "libcpp/data/database.hpp"
#include "libcpp/data/date.hpp"
#include "libcpp/data/pool.hpp"

/* ── async ─────────────────────────────────────────────────────────────── */
#include "libcpp/async/persistent_worker.hpp"
#include "libcpp/async/thread.hpp"
#include "libcpp/async/thread_safe_queue.hpp"
#include "libcpp/async/worker_pool.hpp"

/* ── net ───────────────────────────────────────────────────────────────── */
#include "libcpp/net/client.hpp"
#include "libcpp/net/message.hpp"
#include "libcpp/net/server.hpp"

/* ── math ──────────────────────────────────────────────────────────────── */
#include "libcpp/math/ivector2.hpp"
#include "libcpp/math/ivector3.hpp"
#include "libcpp/math/perlin_noise_2d.hpp"
#include "libcpp/math/random_2d_coordinate_generator.hpp"

#endif /* LIBCPP_LIBCPP_HPP */
