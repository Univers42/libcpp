/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LeakGuard.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/06 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LeakGuard.hpp"
#include <cstdlib>    /* malloc, free                    */
#include <cstdio>     /* fprintf, stderr                 */
#include <new>        /* std::bad_alloc                  */

/* ════════════════════════════════════════════════════════════════════════════
 *  Static member initialisation
 * ═══════════════════════════════════════════════════════════════════════════ */

LgRecord LeakGuard::_ledger[LG_MAX_ALLOCS];
int      LeakGuard::_count          = 0;
int      LeakGuard::_seqCounter     = 0;
int      LeakGuard::_snapshotSeq    = 0;
int      LeakGuard::_snapshotNew    = 0;
int      LeakGuard::_snapshotDel    = 0;
bool     LeakGuard::_enabled        = false;
bool     LeakGuard::_overflowWarned = false;
int      LeakGuard::_totalNew       = 0;
int      LeakGuard::_totalDelete    = 0;
int      LeakGuard::_doubleFrees    = 0;
int      LeakGuard::_wildFrees      = 0;

/* ════════════════════════════════════════════════════════════════════════════
 *  Lifecycle
 * ═══════════════════════════════════════════════════════════════════════════ */

void LeakGuard::enable()  { _enabled = true;  }
void LeakGuard::disable() { _enabled = false; }
bool LeakGuard::isEnabled() { return _enabled; }

void LeakGuard::reset() {
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        _ledger[i].ptr    = NULL;
        _ledger[i].size   = 0;
        _ledger[i].seqId  = 0;
        _ledger[i].active = false;
    }
    _count          = 0;
    _seqCounter     = 0;
    _snapshotSeq    = 0;
    _snapshotNew    = 0;
    _snapshotDel    = 0;
    _overflowWarned = false;
    _totalNew       = 0;
    _totalDelete    = 0;
    _doubleFrees    = 0;
    _wildFrees      = 0;
}

/* ════════════════════════════════════════════════════════════════════════════
 *  Ledger helpers
 * ═══════════════════════════════════════════════════════════════════════════ */

int LeakGuard::findSlot(void* ptr) {
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (_ledger[i].ptr == ptr && _ledger[i].active)
            return i;
    }
    /* also check inactive slots for double-free detection */
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (_ledger[i].ptr == ptr && !_ledger[i].active && _ledger[i].seqId > 0)
            return -(i + 1);  /* negative = found but already freed */
    }
    return -1;
}

int LeakGuard::freeSlot() {
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (!_ledger[i].active && _ledger[i].seqId == 0)
            return i;
    }
    /* no clean slot; reuse any inactive slot */
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (!_ledger[i].active)
            return i;
    }
    return -1;
}

/* ════════════════════════════════════════════════════════════════════════════
 *  Record new / delete
 * ═══════════════════════════════════════════════════════════════════════════ */

void LeakGuard::recordNew(void* ptr, std::size_t size) {
    if (!_enabled || !ptr) return;
    ++_totalNew;
    ++_seqCounter;

    int idx = freeSlot();
    if (idx < 0) {
        if (!_overflowWarned) {
            std::fprintf(stderr,
                "\033[38;5;210m[LeakGuard]\033[0m ledger full (%d slots). "
                "Further allocations won't be tracked.\n", LG_MAX_ALLOCS);
            _overflowWarned = true;
        }
        return;
    }
    _ledger[idx].ptr    = ptr;
    _ledger[idx].size   = size;
    _ledger[idx].seqId  = _seqCounter;
    _ledger[idx].active = true;
    ++_count;
}

int LeakGuard::recordDelete(void* ptr) {
    if (!_enabled || !ptr) return 0;
    ++_totalDelete;

    int raw = findSlot(ptr);

    /* positive → active allocation found, normal free */
    if (raw >= 0) {
        _ledger[raw].active = false;
        --_count;
        return 0;
    }

    /* negative (but not -1) → slot found but already freed → double-free */
    if (raw < -1) {
        ++_doubleFrees;
        std::fprintf(stderr,
            "\033[38;5;210m[LeakGuard]\033[0m \033[1mDOUBLE FREE\033[0m "
            "ptr=%p  (alloc #%d, %zu bytes)\n",
            ptr,
            _ledger[-(raw + 1)].seqId,
            _ledger[-(raw + 1)].size);
        return 1;
    }

    /* raw == -1 → pointer was never tracked → wild free */
    ++_wildFrees;
    /* Wild frees are common for static/global std::string internals,
     * so we only warn if tracking is on and this is a "real" pointer. */
    return 2;
}

/* ════════════════════════════════════════════════════════════════════════════
 *  Snapshot / diff
 * ═══════════════════════════════════════════════════════════════════════════ */

void LeakGuard::snapshot() {
    _snapshotSeq = _seqCounter;
    _snapshotNew = _totalNew;
    _snapshotDel = _totalDelete;
}

int LeakGuard::countLeaksSinceSnapshot() {
    int n = 0;
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (_ledger[i].active && _ledger[i].seqId > _snapshotSeq)
            ++n;
    }
    return n;
}

int LeakGuard::netBalanceSinceSnapshot() {
    int newSince = _totalNew    - _snapshotNew;
    int delSince = _totalDelete - _snapshotDel;
    return newSince - delSince;
}

int LeakGuard::countAllLeaks() {
    int n = 0;
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (_ledger[i].active)
            ++n;
    }
    return n;
}

std::size_t LeakGuard::leakedBytes() {
    std::size_t total = 0;
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (_ledger[i].active)
            total += _ledger[i].size;
    }
    return total;
}

std::size_t LeakGuard::leakedBytesSinceSnapshot() {
    std::size_t total = 0;
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (_ledger[i].active && _ledger[i].seqId > _snapshotSeq)
            total += _ledger[i].size;
    }
    return total;
}

/* ════════════════════════════════════════════════════════════════════════════
 *  Diagnostics
 * ═══════════════════════════════════════════════════════════════════════════ */

int LeakGuard::totalNewCalls()    { return _totalNew;    }
int LeakGuard::totalDeleteCalls() { return _totalDelete; }
int LeakGuard::doubleFreeCount()  { return _doubleFrees; }
int LeakGuard::wildFreeCount()    { return _wildFrees;   }

static void printRecord(const LgRecord& r) {
    std::fprintf(stderr,
        "    alloc \033[1m#%d\033[0m  ptr=\033[38;5;141m%p\033[0m  "
        "size=\033[38;5;183m%zu\033[0m bytes\n",
        r.seqId, r.ptr, r.size);
}

void LeakGuard::reportLeaks() {
    int leaks = countAllLeaks();
    if (leaks == 0) {
        std::fprintf(stderr,
            "\033[38;5;114m[LeakGuard]\033[0m "
            "\033[1mNo leaks detected.\033[0m  "
            "(new=%d  delete=%d)\n",
            _totalNew, _totalDelete);
        return;
    }
    std::fprintf(stderr,
        "\n\033[38;5;210m[LeakGuard]\033[0m "
        "\033[1m%d leaked allocation(s)\033[0m  "
        "(%zu bytes):\n",
        leaks, leakedBytes());
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (_ledger[i].active)
            printRecord(_ledger[i]);
    }
    std::fprintf(stderr,
        "  \033[2m(new=%d  delete=%d  double-free=%d  wild-free=%d)\033[0m\n\n",
        _totalNew, _totalDelete, _doubleFrees, _wildFrees);
}

void LeakGuard::reportLeaksSinceSnapshot() {
    int leaks = countLeaksSinceSnapshot();
    if (leaks == 0) {
        std::fprintf(stderr,
            "\033[38;5;114m[LeakGuard]\033[0m "
            "\033[1mNo leaks since snapshot.\033[0m  "
            "(new=%d  delete=%d  snapshot@#%d)\n",
            _totalNew, _totalDelete, _snapshotSeq);
        return;
    }
    std::fprintf(stderr,
        "\n\033[38;5;210m[LeakGuard]\033[0m "
        "\033[1m%d leaked allocation(s) since snapshot #%d\033[0m  "
        "(%zu bytes):\n",
        leaks, _snapshotSeq, leakedBytesSinceSnapshot());
    for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
        if (_ledger[i].active && _ledger[i].seqId > _snapshotSeq)
            printRecord(_ledger[i]);
    }
    std::fprintf(stderr,
        "  \033[2m(new=%d  delete=%d  double-free=%d  wild-free=%d)\033[0m\n\n",
        _totalNew, _totalDelete, _doubleFrees, _wildFrees);
}

/* ════════════════════════════════════════════════════════════════════════════
 *  Global operator new / delete overrides
 *
 *  These intercept ALL heap allocations in the program.
 *  When LeakGuard is disabled they are essentially just malloc/free.
 *  When enabled they also feed the ledger.
 *
 *  NOTE: These were previously in postman.cpp.  Having them here keeps all
 *        memory-tracking logic in one translation unit.
 * ═══════════════════════════════════════════════════════════════════════════ */

void* operator new(std::size_t size) {
    void* ptr = std::malloc(size);
    if (!ptr)
        throw std::bad_alloc();
    LeakGuard::recordNew(ptr, size);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    if (!ptr) return;
    LeakGuard::recordDelete(ptr);
    std::free(ptr);
}

void* operator new[](std::size_t size) {
    void* ptr = std::malloc(size);
    if (!ptr)
        throw std::bad_alloc();
    LeakGuard::recordNew(ptr, size);
    return ptr;
}

void operator delete[](void* ptr) noexcept {
    if (!ptr) return;
    LeakGuard::recordDelete(ptr);
    std::free(ptr);
}
