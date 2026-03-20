/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 21:18:42 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 21:18:42 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include "colors.hpp" // add palette

namespace tester
{
	class Message
	{
	public:
		typedef std::ostream &(*BasicNarrowIoManip)(std::ostream &);

		Message() : ss_(new std::stringstream), color_prefix_() {}

		// copy
		Message(const Message &msg) : ss_(new std::stringstream), color_prefix_(msg.color_prefix_)
		{
			*ss_ << msg.GetString();
		}

		// move
		Message(Message &&other) noexcept
			: ss_(std::move(other.ss_)), color_prefix_(std::move(other.color_prefix_))
		{
			// other left in a valid empty state
			if (!ss_)
				ss_.reset(new std::stringstream);
		}

		explicit Message(const char *str) : ss_(new std::stringstream), color_prefix_()
		{
			*ss_ << str;
		}

		template <typename T>
		inline Message &operator<<(const T &val)
		{
			*ss_ << val;
			return *this;
		}

		template <typename T>
		inline Message &operator<<(T *const &pointer)
		{
			if (pointer == nullptr)
				*ss_ << "(null)";
			else
				*ss_ << pointer;
			return *this;
		}

		Message &operator<<(BasicNarrowIoManip val)
		{
			*ss_ << val;
			return *this;
		}

		Message &operator<<(bool b)
		{
			return (*this << (b ? "true" : "false"));
		}

		// narrow conversions from wide C-strings: simple lossless conversion for ASCII-range chars
		Message &operator<<(const wchar_t *wide_c_str)
		{
			if (wide_c_str == nullptr)
				*ss_ << "(null)";
			else
			{
				std::wstring ws(wide_c_str);
				std::string s(ws.begin(), ws.end());
				*ss_ << s;
			}
			return *this;
		}

		Message &operator<<(wchar_t *wide_c_str)
		{
			return (*this << static_cast<const wchar_t *>(wide_c_str));
		}

		std::string GetString() const
		{
			return ss_ ? ss_->str() : std::string();
		}

		// explicit conversion to std::string for logger compatibility
		explicit operator std::string() const { return GetString(); }

		// color control
		inline Message &SetColor(const char *color_code)
		{
			color_prefix_ = color_code ? color_code : "";
			return *this;
		}
		inline Message &SetColor(const std::string &color_code)
		{
			color_prefix_ = color_code;
			return *this;
		}
		inline Message &ClearColor()
		{
			color_prefix_.clear();
			return *this;
		}

		// allow streaming Message to ostream (wrap with color if present)
		friend std::ostream &operator<<(std::ostream &os, const Message &m)
		{
			if (!m.color_prefix_.empty())
				return (os << m.color_prefix_ << m.GetString() << tester::colors::RESET);
			return (os << m.GetString());
		}

	private:
		std::unique_ptr<std::stringstream> ss_;
		std::string color_prefix_;
		Message &operator=(const Message &) = delete;
	};

	namespace internal
	{
		template <typename T>
		std::string StreamableToString(const T &streamable)
		{
			Message m;
			m << streamable;
			return m.GetString();
		}
	} // namespace internal

} // namespace tester

#endif