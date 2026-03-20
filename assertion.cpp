/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assertion.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 21:16:28 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 21:16:28 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "assertion.hpp"

#include <string>
#include <utility>
#include <cstring>
#include <cmath>
#include <iostream>

#include "assertion.hpp"
#include "message.hpp"

namespace testing
{

    AssertionResult::AssertionResult(bool success)
        : success_(success), message_(nullptr) {}

    AssertionResult::AssertionResult(const AssertionResult &other)
        : success_(other.success_), message_(nullptr)
    {
        if (other.message_)
            message_ = new std::string(*other.message_);
    }

    AssertionResult::~AssertionResult()
    {
        delete message_;
    }

    AssertionResult &AssertionResult::operator=(AssertionResult other)
    {
        swap(other);
        return *this;
    }

    void AssertionResult::swap(AssertionResult &other)
    {
        using std::swap;
        swap(success_, other.success_);
        swap(message_, other.message_);
    }

    AssertionResult AssertionResult::operator!() const
    {
        AssertionResult neg(!success_);
        if (message_)
            neg << *message_;
        return neg;
    }

    AssertionResult &AssertionResult::operator<<(const std::string &s)
    {
        if (!message_)
            message_ = new std::string;
        *message_ += s;
        return *this;
    }

    AssertionResult &AssertionResult::operator<<(const tester::Message &m)
    {
        return (*this << m.GetString());
    }

    bool AssertionResult::success() const { return success_; }

    std::string AssertionResult::message() const
    {
        return message_ ? *message_ : std::string();
    }

    AssertionResult AssertionSuccess()
    {
        return AssertionResult(true);
    }

    AssertionResult AssertionFailure()
    {
        return AssertionResult(false);
    }

    AssertionResult AssertionFailure(const tester::Message &message)
    {
        AssertionResult r = AssertionFailure();
        r << message;
        return r;
    }

} // namespace testing

// C API wrappers for simple assertions used by C test programs
extern "C"
{

    // compare ints
    int c_assert_int_eq(int a, int b, const char *name)
    {
        bool ok = (a == b);
        tester::Message m;
        if (ok)
            m.SetColor(tester::colors::GREEN) << "[ OK ] " << name << " - " << a;
        else
            m.SetColor(tester::colors::BG_BRIGHT_RED) << "[FAIL] " << name << " - expected=" << a << " got=" << b;
        std::cout << m << std::endl;
        return ok ? 1 : 0;
    }

    // compare unsigned ints
    int c_assert_uint_eq(unsigned a, unsigned b, const char *name)
    {
        bool ok = (a == b);
        tester::Message m;
        if (ok)
            m.SetColor(tester::colors::GREEN) << "[ OK ] " << name << " - " << a;
        else
            m.SetColor(tester::colors::BG_BRIGHT_RED) << "[FAIL] " << name << " - expected=" << a << " got=" << b;
        std::cout << m << std::endl;
        return ok ? 1 : 0;
    }

    // approximate double comparison
    int c_assert_double_approx(double a, double b, double tol, const char *name)
    {
        bool ok = std::fabs(a - b) <= tol;
        tester::Message m;
        if (ok)
            m.SetColor(tester::colors::GREEN) << "[ OK ] " << name << " - ≈ " << a;
        else
            m.SetColor(tester::colors::BG_BRIGHT_RED) << "[FAIL] " << name << " - expected≈" << b << " got=" << a;
        std::cout << m << std::endl;
        return ok ? 1 : 0;
    }

    // strcmp-based string equality (null-safe)
    int c_assert_str_eq(const char *a, const char *b, const char *name)
    {
        bool ok;
        if (a == NULL && b == NULL)
            ok = true;
        else if (a == NULL || b == NULL)
            ok = false;
        else
            ok = (strcmp(a, b) == 0);
        tester::Message m;
        if (ok)
            m.SetColor(tester::colors::GREEN) << "[ OK ] " << name << " - " << (a ? a : "(null)");
        else
            m.SetColor(tester::colors::BG_BRIGHT_RED) << "[FAIL] " << name << " - expected=\"" << (b ? b : "(null)") << "\" got=\"" << (a ? a : "(null)") << "\"";
        std::cout << m << std::endl;
        return ok ? 1 : 0;
    }

    // memcmp-based buffer equality
    int c_assert_memcmp(const void *a, const void *b, size_t n, const char *name)
    {
        bool ok = (a != NULL && b != NULL && n == 0) ? true : (a != NULL && b != NULL ? (memcmp(a, b, n) == 0) : false);
        tester::Message m;
        if (ok)
            m.SetColor(tester::colors::GREEN) << "[ OK ] " << name << " - memcmp(" << n << ")";
        else
            m.SetColor(tester::colors::BG_BRIGHT_RED) << "[FAIL] " << name << " - memcmp(" << n << ") mismatch";
        std::cout << m << std::endl;
        return ok ? 1 : 0;
    }

    // pointer equality
    int c_assert_ptr_eq(const void *a, const void *b, const char *name)
    {
        bool ok = (a == b);
        tester::Message m;
        if (ok)
            m.SetColor(tester::colors::GREEN) << "[ OK ] " << name << " - ptr=" << a;
        else
            m.SetColor(tester::colors::BG_BRIGHT_RED) << "[FAIL] " << name << " - ptrs differ (" << a << " != " << b << ")";
        std::cout << m << std::endl;
        return ok ? 1 : 0;
    }

    // formatted expected / unexpected helpers
    void c_print_expected(const char *name)
    {
        tester::Message m;
        // blue + italic
        m.SetColor(std::string(tester::colors::ITALIC) + tester::colors::BLUE) << "[EXPECTED] " << (name ? name : "");
        std::cout << m << std::endl;
    }

    void c_print_unexpected_pass(const char *name)
    {
        tester::Message m;
        // bright yellow + italic
        m.SetColor(std::string(tester::colors::ITALIC) + tester::colors::BRIGHT_YELLOW) << "[UNEXPECTED PASS] " << (name ? name : "");
        std::cout << m << std::endl;
    }

} // extern "C"