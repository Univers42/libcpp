/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reactor.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP98_REACTOR_HPP
#define LIBCPP98_REACTOR_HPP

namespace libcpp98 {

/* ── Thin epoll registration wrapper (Linux) ───────────────────────────────
**
** Owns the epoll instance and the EPOLL_CTL bookkeeping — with every return
** value checked, which raw call sites habitually forget.  Deliberately does
** NOT wrap the wait call: an event-loop owner (e.g. a 42 network project
** required to show its single poll-equivalent call site) keeps the literal
** epoll_wait(reactor.fd(), ...) in its own source.
*/
class Reactor {
public:
	Reactor();
	~Reactor();

	/* Creates the epoll instance.  Returns false on failure (errno set). */
	bool	open();
	bool	ok() const;
	void	close();

	/* The epoll fd, for the caller's own epoll_wait(). */
	int		fd() const;

	/* EPOLL_CTL_ADD / MOD / DEL.  Return false on failure (errno set). */
	bool	add(int fd, unsigned events);
	bool	modify(int fd, unsigned events);
	bool	remove(int fd);

private:
	int	_epfd;

	Reactor(const Reactor &);
	Reactor &operator=(const Reactor &);
};

} /* namespace libcpp98 */

#endif /* LIBCPP98_REACTOR_HPP */
