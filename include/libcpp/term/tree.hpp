/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:27:52 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_TREE_HPP
# define LIBCPP_TERM_TREE_HPP

# include <string>
# include <vector>
# include <sstream>
# include <iostream>
# include "libcpp/term/color.hpp"

namespace libcpp
{

/*
** TreeNode — hierarchical tree node
** Orthodox Canonical Form compliant.
*/
class TreeNode
{
public:
	std::string             label;
	std::string             detail;
	Srgb                    label_color;
	Srgb                    detail_color;
	std::vector<TreeNode>   children;

	/* ── OCF ───────────────────────────────────────────────────── */
	TreeNode();
	TreeNode(const std::string& lbl);
	TreeNode(const std::string& lbl, const std::string& det);
	TreeNode(const TreeNode& other);
	TreeNode& operator=(const TreeNode& other);
	~TreeNode();

	/* ── builder API ───────────────────────────────────────────── */
	TreeNode& add(const std::string& lbl, const std::string& det = "");
	TreeNode& child(const TreeNode& node);
};

/*
** TreeStyle — configuration for tree rendering
*/
struct TreeStyle
{
	std::string branch;
	std::string last;
	std::string pipe;
	std::string blank;
	bool        show_counts;
	Srgb        branch_color;
	int         max_depth;

	TreeStyle();
	TreeStyle(const TreeStyle& other);
	TreeStyle& operator=(const TreeStyle& other);
	~TreeStyle();
};

/*
** TreeRenderer — renders tree structures with box-drawing characters
** Orthodox Canonical Form compliant.
*/
class TreeRenderer
{
public:
	/* ── OCF ───────────────────────────────────────────────────── */
	TreeRenderer();
	explicit TreeRenderer(const TreeStyle& style);
	TreeRenderer(const TreeRenderer& other);
	TreeRenderer& operator=(const TreeRenderer& other);
	~TreeRenderer();

	std::string render(const TreeNode& root) const;
	std::string to_json(const TreeNode& root) const;
	void print(const TreeNode& root) const;

private:
	TreeStyle _style;

	void _render_node(std::ostringstream& out, const TreeNode& node,
		const std::string& prefix, bool is_last, int depth) const;
};

} /* namespace libcpp */

#endif /* LIBCPP_TERM_TREE_HPP */
