/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reactor.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp98/reactor.hpp"

#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

namespace libcpp98 {

Reactor::Reactor()
	: _epfd(-1)
{
}

Reactor::~Reactor()
{
	close();
}

bool Reactor::open()
{
	if (_epfd >= 0)
		return true;
	_epfd = epoll_create1(0);
	return _epfd >= 0;
}

bool Reactor::ok() const
{
	return _epfd >= 0;
}

void Reactor::close()
{
	if (_epfd >= 0)
	{
		::close(_epfd);
		_epfd = -1;
	}
}

int Reactor::fd() const
{
	return _epfd;
}

bool Reactor::add(int fd, unsigned events)
{
	struct epoll_event ev;
	std::memset(&ev, 0, sizeof(ev));
	ev.events = events;
	ev.data.fd = fd;
	return epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool Reactor::modify(int fd, unsigned events)
{
	struct epoll_event ev;
	std::memset(&ev, 0, sizeof(ev));
	ev.events = events;
	ev.data.fd = fd;
	return epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) == 0;
}

bool Reactor::remove(int fd)
{
	return epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL) == 0;
}

} /* namespace libcpp98 */
