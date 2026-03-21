/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 22:06:04 by dlesieur         ###   ########.fr       */
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

int TreeNode::depth() const
{
	int max_d = 0;
	for (std::size_t i = 0; i < children.size(); ++i)
	{
		int d = children[i].depth() + 1;
		if (d > max_d) max_d = d;
	}
	return max_d;
}

int TreeNode::count() const
{
	int n = 1;
	for (std::size_t i = 0; i < children.size(); ++i)
		n += children[i].count();
	return n;
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

void TreeRenderer::set_style(const TreeStyle& s) { _style = s; }

static void _json_escape(std::ostringstream& out, const std::string& s)
{
	for (std::size_t i = 0; i < s.size(); ++i)
	{
		char c = s[i];
		if (c == '"') out << "\\\"";
		else if (c == '\\') out << "\\\\";
		else if (c == '\n') out << "\\n";
		else if (c == '\r') out << "\\r";
		else if (c == '\t') out << "\\t";
		else out << c;
	}
}

static void _node_to_json(std::ostringstream& out, const TreeNode& node,
	int indent)
{
	std::string pad(static_cast<std::string::size_type>(indent * 2), ' ');
	std::string pad2(static_cast<std::string::size_type>((indent + 1) * 2), ' ');
	out << pad << "{\n";
	out << pad2 << "\"label\": \"";
	_json_escape(out, node.label);
	out << "\"";
	if (!node.detail.empty())
	{
		out << ",\n" << pad2 << "\"detail\": \"";
		_json_escape(out, node.detail);
		out << "\"";
	}
	if (!node.children.empty())
	{
		out << ",\n" << pad2 << "\"children\": [\n";
		for (std::size_t i = 0; i < node.children.size(); ++i)
		{
			_node_to_json(out, node.children[i], indent + 2);
			if (i + 1 < node.children.size())
				out << ",";
			out << "\n";
		}
		out << pad2 << "]";
	}
	out << "\n" << pad << "}";
}

std::string TreeRenderer::to_json(const TreeNode& root) const
{
	std::ostringstream out;
	_node_to_json(out, root, 0);
	out << "\n";
	return out.str();
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
