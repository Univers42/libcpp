/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/tree.hpp"
#include "libcpp/term/style.hpp"

namespace libcpp
{

/* ══════════════════════════════════════════════════════════════════════════
 *  TreeNode
 * ═════════════════════════════════════════════════════════════════════════ */

TreeNode::TreeNode() : label_color(204, 204, 204), detail_color(128, 128, 128) {}

TreeNode::TreeNode(const std::string& lbl)
	: label(lbl), label_color(204, 204, 204), detail_color(128, 128, 128) {}

TreeNode::TreeNode(const std::string& lbl, const std::string& det)
	: label(lbl), detail(det), label_color(204, 204, 204),
	  detail_color(128, 128, 128) {}

TreeNode::TreeNode(const TreeNode& o)
	: label(o.label), detail(o.detail), label_color(o.label_color),
	  detail_color(o.detail_color), children(o.children) {}

TreeNode& TreeNode::operator=(const TreeNode& o)
{
	if (this != &o)
	{
		label = o.label; detail = o.detail;
		label_color = o.label_color; detail_color = o.detail_color;
		children = o.children;
	}
	return *this;
}

TreeNode::~TreeNode() {}

TreeNode& TreeNode::add(const std::string& lbl, const std::string& det)
{
	children.push_back(TreeNode(lbl, det));
	return children.back();
}

TreeNode& TreeNode::child(const TreeNode& node)
{
	children.push_back(node);
	return children.back();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  TreeStyle
 * ═════════════════════════════════════════════════════════════════════════ */

TreeStyle::TreeStyle()
	: branch("\xe2\x94\x9c\xe2\x94\x80\xe2\x94\x80 "),
	  last("\xe2\x94\x94\xe2\x94\x80\xe2\x94\x80 "),
	  pipe("\xe2\x94\x82   "),
	  blank("    "),
	  show_counts(false),
	  branch_color(128, 128, 128),
	  max_depth(-1) {}

TreeStyle::TreeStyle(const TreeStyle& o)
	: branch(o.branch), last(o.last), pipe(o.pipe), blank(o.blank),
	  show_counts(o.show_counts), branch_color(o.branch_color),
	  max_depth(o.max_depth) {}

TreeStyle& TreeStyle::operator=(const TreeStyle& o)
{
	if (this != &o)
	{
		branch = o.branch; last = o.last; pipe = o.pipe; blank = o.blank;
		show_counts = o.show_counts; branch_color = o.branch_color;
		max_depth = o.max_depth;
	}
	return *this;
}

TreeStyle::~TreeStyle() {}

/* ══════════════════════════════════════════════════════════════════════════
 *  TreeRenderer
 * ═════════════════════════════════════════════════════════════════════════ */

TreeRenderer::TreeRenderer() {}

TreeRenderer::TreeRenderer(const TreeStyle& style) : _style(style) {}

TreeRenderer::TreeRenderer(const TreeRenderer& o) : _style(o._style) {}

TreeRenderer& TreeRenderer::operator=(const TreeRenderer& o)
{
	if (this != &o)
		_style = o._style;
	return *this;
}

TreeRenderer::~TreeRenderer() {}

std::string TreeRenderer::render(const TreeNode& root) const
{
	std::ostringstream out;
	out << root.label_color.to_ansi_fg() << root.label;
	if (!root.detail.empty())
		out << " " << root.detail_color.to_ansi_fg() << root.detail;
	out << TermUtils::reset() << "\n";

	for (std::size_t i = 0; i < root.children.size(); ++i)
	{
		bool is_last = (i == root.children.size() - 1);
		_render_node(out, root.children[i], "", is_last, 1);
	}
	return out.str();
}

void TreeRenderer::print(const TreeNode& root) const
{
	std::cout << render(root);
}

void TreeRenderer::_render_node(std::ostringstream& out, const TreeNode& node,
	const std::string& prefix, bool is_last, int depth) const
{
	if (_style.max_depth >= 0 && depth > _style.max_depth)
		return;

	out << _style.branch_color.to_ansi_fg() << prefix;
	out << (is_last ? _style.last : _style.branch);
	out << TermUtils::reset();
	out << node.label_color.to_ansi_fg() << node.label;
	if (!node.detail.empty())
		out << " " << node.detail_color.to_ansi_fg() << node.detail;
	if (_style.show_counts && !node.children.empty())
	{
		std::ostringstream cnt;
		cnt << " (" << node.children.size() << ")";
		out << node.detail_color.to_ansi_fg() << cnt.str();
	}
	out << TermUtils::reset() << "\n";

	std::string new_prefix = prefix + (is_last ? _style.blank : _style.pipe);
	for (std::size_t i = 0; i < node.children.size(); ++i)
	{
		bool child_last = (i == node.children.size() - 1);
		_render_node(out, node.children[i], new_prefix, child_last, depth + 1);
	}
}

} /* namespace libcpp */
