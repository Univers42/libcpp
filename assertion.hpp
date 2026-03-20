/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assertion.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 18:32:37 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 18:32:37 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASSERTION_HPP
# define ASSERTION_HPP

#ifndef TESTING_ASSERT_HPP
#define TESTING_ASSERT_HPP
#pragma once

#include <string>
#include "message.hpp"

namespace testing
{

    class AssertionResult
    {
    public:
        AssertionResult(bool success = true);
        AssertionResult(const AssertionResult &other);
        ~AssertionResult();

        AssertionResult &operator=(AssertionResult other);
        void swap(AssertionResult &other);

        // logical negation: returns a result with inverted success flag, preserving message
        AssertionResult operator!() const;

        // append text or Message
        AssertionResult &operator<<(const std::string &s);
        AssertionResult &operator<<(const tester::Message &m);

        // accessors
        bool success() const;
        std::string message() const;

    private:
        bool success_;
        std::string *message_;
    };

    AssertionResult AssertionSuccess();
    AssertionResult AssertionFailure();
    AssertionResult AssertionFailure(const tester::Message &message);

} // namespace testing

#endif // TESTING_ASSERT_HPP
#endif