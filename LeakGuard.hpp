/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LeakGuard.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/06 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEAKGUARD_HPP
# define LEAKGUARD_HPP

/**
 * @file LeakGuard.hpp
 * @brief Runtime memory-leak detector & diagnostic tool for C++98.
 *
 * LeakGuard intercepts every `operator new` / `operator delete` and maintains
 * a fixed-capacity ledger of live allocations.  It can:
 *
 *   1. **Detect actual leaks** — allocations that were never freed.
 *   2. **Detect double-free** — calling delete on an already-freed pointer.
 *   3. **Detect wild free** — calling delete on a pointer never returned by new.
 *   4. **Snapshot / diff** — take a baseline, then check that all allocations
 *      made after the snapshot have been freed (scoped leak checks).
 *   5. **Report** — dump a human-readable summary to stderr.
 *
 * ## Standalone usage (no postman)
 * @code
 *   LeakGuard::enable();
 *   LeakGuard::snapshot();
 *   // … code under test …
 *   int leaks = LeakGuard::countLeaksSinceSnapshot();
 *   LeakGuard::reportLeaks();
 *   LeakGuard::disable();
 * @endcode
 *
 * ## Postman integration
 * The existing `TestReport::snapshotMemory()` and `assertNoLeaks()` methods
 * delegate to LeakGuard automatically when it is enabled.
 *
 * @note All state is static — no instance needed, no dynamic allocation
 *       inside the tracker itself (fixed arrays only).
 *
 * @warning The ledger has a hard capacity of LG_MAX_ALLOCS.  If your program
 *          exceeds that, new allocations are still serviced by malloc() but
 *          won't be tracked.  A warning is emitted to stderr once.
 */

# include <cstddef>   /* std::size_t */
# include <string>

/** Maximum simultaneous tracked allocations (tune for your program). */
# define LG_MAX_ALLOCS 4096

/**
 * @struct LgRecord
 * @brief One slot in the allocation ledger.
 *
 * @var ptr      The pointer returned by malloc (NULL if slot is free).
 * @var size     Allocation size in bytes.
 * @var seqId    Monotonic allocation number (1-indexed).
 * @var active   True while the allocation is live, false after delete.
 */
struct LgRecord {
    void*       ptr;
    std::size_t size;
    int         seqId;
    bool        active;
};

/**
 * @class LeakGuard
 * @brief Static utility — runtime allocation tracker for C++98.
 *
 * Maintains a fixed-size array of LgRecord entries.  Slots are reused once
 * freed, keeping memory overhead constant regardless of total allocation count.
 *
 * ### Design constraints (C++98, 42 School)
 * - No STL containers (std::map, std::set, std::vector) — plain arrays only.
 * - No placement new inside the tracker — avoids recursion with overridden new.
 * - All internal storage is static; no heap usage by LeakGuard itself.
 */
class LeakGuard {
 public:
    /* ── lifecycle ────────────────────────────────────────── */

    /** Enable tracking.  operator new/delete will start recording. */
    static void enable();

    /** Disable tracking.  operator new/delete pass straight to malloc/free. */
    static void disable();

    /** @return true if tracking is currently active. */
    static bool isEnabled();

    /* ── allocation ledger ────────────────────────────────── */

    /**
     * @brief Register a new allocation.
     * Called from the global operator new override.
     * @param ptr  Pointer returned by malloc.
     * @param size Requested allocation size.
     */
    static void recordNew(void* ptr, std::size_t size);

    /**
     * @brief Register a deallocation.
     * Called from the global operator delete override.
     * @param ptr  Pointer being freed.
     * @return  0 = ok, 1 = double-free, 2 = wild-free (unknown ptr).
     */
    static int  recordDelete(void* ptr);

    /* ── snapshot / diff (scoped leak detection) ─────────── */

    /** Save a baseline: the current sequence counter value. */
    static void snapshot();

    /**
     * @brief Count allocations made *after* the last snapshot that are
     *        still live (never freed).
     */
    static int  countLeaksSinceSnapshot();

    /**
     * @brief Net balance of new vs delete calls since the last snapshot.
     *
     * Returns (newSinceSnapshot - deleteSinceSnapshot).  A positive value
     * means more allocations than deallocations.  This is more tolerant of
     * framework noise (e.g. lazy static-local initialisation) because it
     * also counts delete calls on pointers allocated before the snapshot.
     */
    static int  netBalanceSinceSnapshot();

    /**
     * @brief Count ALL live (never freed) allocations in the ledger,
     *        regardless of snapshot.
     */
    static int  countAllLeaks();

    /** @return Total bytes still live across ALL tracked allocations. */
    static std::size_t leakedBytes();

    /** @return Total bytes still live since last snapshot. */
    static std::size_t leakedBytesSinceSnapshot();

    /* ── diagnostics ──────────────────────────────────────── */

    /** @return Total number of new() calls seen since enable(). */
    static int  totalNewCalls();

    /** @return Total number of delete() calls seen since enable(). */
    static int  totalDeleteCalls();

    /** @return Number of double-free events detected so far. */
    static int  doubleFreeCount();

    /** @return Number of wild-free events (delete on unknown ptr). */
    static int  wildFreeCount();

    /**
     * @brief Print a detailed leak report to stderr.
     *
     * Lists every live allocation (ptr, size, seqId) and then a summary
     * line with totals.  Designed to be human-readable in a terminal.
     */
    static void reportLeaks();

    /**
     * @brief Print leaks since last snapshot only.
     */
    static void reportLeaksSinceSnapshot();

    /**
     * @brief Reset the entire ledger.  Useful between independent test runs.
     */
    static void reset();

 private:
    /* no instances */
    LeakGuard();
    ~LeakGuard();
    LeakGuard(const LeakGuard&);
    LeakGuard& operator=(const LeakGuard&);

    /* ── internal state (all static, fixed-size) ─────────── */
    static LgRecord  _ledger[LG_MAX_ALLOCS];
    static int       _count;          /* number of occupied slots            */
    static int       _seqCounter;     /* monotonic allocation ID             */
    static int       _snapshotSeq;    /* seqId at last snapshot()            */
    static int       _snapshotNew;    /* _totalNew at last snapshot()        */
    static int       _snapshotDel;    /* _totalDelete at last snapshot()     */
    static bool      _enabled;        /* tracking on/off                     */
    static bool      _overflowWarned; /* emit overflow warning only once     */
    static int       _totalNew;       /* lifetime new() count                */
    static int       _totalDelete;    /* lifetime delete() count             */
    static int       _doubleFrees;    /* double-free events detected         */
    static int       _wildFrees;      /* delete-on-unknown events detected   */

    /**
     * @brief Find the ledger slot for a given pointer.
     * @return Index into _ledger, or -1 if not found.
     */
    static int findSlot(void* ptr);

    /**
     * @brief Find the first free (inactive) slot, or -1 if full.
     */
    static int freeSlot();
};

#endif /* LEAKGUARD_HPP */
