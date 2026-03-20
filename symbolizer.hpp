/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbolizer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 21:21:03 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 21:21:03 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>

namespace symbolizer
{
    // symbolize a single address (returns one-line "func at file:line" or raw hex if unavailable)
    std::string symbolize(void *addr);

    // symbolize multiple addresses (skip 'skip' frames)
    std::vector<std::string> symbolize_many(const std::vector<void *> &addrs, int skip = 0);
}