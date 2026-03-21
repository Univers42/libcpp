/* ── Backward-compatibility wrapper ───────────────────────────────────── */
/* Old code: #include "libcpp.h"  →  now includes the master header.      */
#ifndef LIBCPP_H
# define LIBCPP_H
# include "libcpp/libcpp.hpp"
#endif
