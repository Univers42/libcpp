/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_math.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 17:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 17:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_math.cpp — 2D game world procedural generation
**
** Scenario: Use IVector2/IVector3 for map coordinates, PerlinNoise2D to
** generate a terrain heightmap, and Random2DCoordinateGenerator to place
** trees on the map. Renders an ASCII terrain heatmap.
**
** Theme: Sunset Blaze (warm orange-pink on dark brown)
*/

#include <cmath>
#include <iomanip>
#include <sstream>

#include "libcpp/math/ivector2.hpp"
#include "libcpp/math/ivector3.hpp"
#include "libcpp/math/perlin_noise_2d.hpp"
#include "libcpp/math/random_2d_coordinate_generator.hpp"
#include "libcpp/str/format.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

using libcpp::Srgb;
using libcpp::StyleSheet;
using libcpp::TermStyle;
using libcpp::TermWriter;

/* ── Sunset Blaze theme ──────────────────────────────────────────────── */

LIBCPP_THEME_BEGIN(sunset_blaze)
  RULE("h1", fg("#FF6B6B").bg("#1A0A0A").border("#FF6B6B")
                 .bold().center().pad(3, 3, 1).spacing(1, 1)
                 .glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90"))
  RULE("h2", fg("#FF9248").border("#331A0A")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))
  RULE("h3", fg("#FFB347").border("#331A0A")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c"))
  RULE("body",    fg("#DEBB99").pad_left(2))
  RULE("quote",   fg("#AA8866").italic().glyph("\xe2\x94\x82 ").margin_left(3))
  RULE("info",    fg("#FF9248").glyph("\xe2\x84\xb9  "))
  RULE("warn",    fg("#FFCC00").bold().glyph("\xe2\x9a\xa0  "))
  RULE("error",   fg("#FF4444").bold().glyph("\xe2\x9c\x97 "))
  RULE("success", fg("#AADD55").glyph("\xe2\x9c\x94 "))
  RULE("trace",   fg("#887755").glyph("   "))
  RULE("sep",     border("#331A0A").sep("\xe2\x94\x80"))
  RULE("bullet",  fg("#DEBB99").glyph("\xe2\x96\xaa ").pad_left(4))
  RULE("ol",      fg("#DEBB99").pad_left(4))
  CALLOUT_RULE("tip",  fg("#FFE0B2").bg("#1A1008").border("#FF9248")
                           .glyph("\xe2\x9c\x94 "))
  CALLOUT_RULE("note", fg("#FFCC80").bg("#1A0D05").border("#FF6B6B")
                           .glyph("\xe2\x84\xb9  "))
  TABLE_RULE(border_preset(libcpp::BORDER_ROUNDED).border_color("#FF9248")
                 .header_fg("#FF6B6B").header_bg("#1A0A0A").header_font(libcpp::FONT_BOLD)
                 .header_has_bg(true)
                 .cell_fg("#DEBB99").cell_bg("#1A0A0A").cell_alt_bg("#201008")
                 .zebra(true).cell_has_bg(true)
                 .title_fg("#FF6B6B").title_bg("#1A0A0A").title_has_bg(true)
                 .footer_fg("#AA8866"))
  WIDTH(74)
LIBCPP_THEME_END

/* helper: double to string with precision */
static std::string dtos(double v, int prec) {
  std::ostringstream o;
  o << std::fixed << std::setprecision(prec) << v;
  return o.str();
}

int main() {
  TermStyle ts;
  sunset_blaze().apply(ts);
  TermWriter w(ts);

  w << "# Procedural World Generation";
  w << "";
  w << "Using `libcpp::math` to build a 2D terrain map with vectors,";
  w << "Perlin noise heightmaps, and random tree placement.";

  // ── IVector2 arithmetic ───────────────────────────────────────────
  w << "## IVector2 — Map Coordinates";

  libcpp::math::IVector2 origin(0, 0);
  libcpp::math::IVector2 town(15, 8);
  libcpp::math::IVector2 dungeon(30, 22);

  libcpp::math::IVector2 path = dungeon - town;
  int dist_sq = path.lengthSquared();
  double dist = path.length();

  w << "| Location | Position | Distance from Town |";
  w << "| :--- | :---: | ---: |";
  {
    std::ostringstream o, t, d;
    o << origin; t << town; d << dungeon;
    w << ("| Origin | " + o.str() + " | -- |");
    w << ("| Town | " + t.str() + " | 0 |");
    w << ("| Dungeon | " + d.str() + " | " + dtos(dist, 1) + " |");
  }
  w << "";

  w << ("!i Path vector: (" + libcpp::str::to_string(path.x) + ", "
      + libcpp::str::to_string(path.y) + ")");
  w << ("!i Distance squared: " + libcpp::str::to_string(dist_sq));
  w << ("!i Dot product (town . dungeon): "
      + libcpp::str::to_string(town.dot(dungeon)));
  w << ("!i Cross product (town x dungeon): "
      + libcpp::str::to_string(town.cross(dungeon)));
  w << "";

  // ── IVector3 ──────────────────────────────────────────────────────
  w << "## IVector3 — 3D World Space";

  libcpp::math::IVector3 camera(10, 5, 20);
  libcpp::math::IVector3 target(30, 15, 5);
  libcpp::math::IVector3 look = target - camera;

  {
    std::ostringstream c, t, l;
    c << camera; t << target; l << look;
    w << ("!i Camera:    " + c.str());
    w << ("!i Target:    " + t.str());
    w << ("!i Look dir:  " + l.str());
    w << ("!i Distance:  " + dtos(look.length(), 2));
  }
  w << "";

  libcpp::math::IVector3 up(0, 1, 0);
  libcpp::math::IVector3 right = look.cross(up);
  {
    std::ostringstream r;
    r << right;
    w << ("!i Cross (look x up): " + r.str() + " (camera right vector)");
  }
  w << "";

  // ── Perlin noise heightmap ────────────────────────────────────────
  w << "## Perlin Noise — Terrain Heightmap";
  w << "16x16 heightmap sampled at scale 0.15, 4 octaves:";
  w << "";

  libcpp::math::PerlinNoise2D noise(42);
  const int MAP = 16;
  double heights[MAP][MAP];
  double hmin = 1e9, hmax = -1e9;

  for (int y = 0; y < MAP; ++y) {
    for (int x = 0; x < MAP; ++x) {
      double h = noise.octave(x * 0.15, y * 0.15, 4, 0.5);
      heights[y][x] = h;
      if (h < hmin) hmin = h;
      if (h > hmax) hmax = h;
    }
  }

  /* Render as ASCII heatmap */
  const char* shades = " .-:=+*#%@";
  int shade_count = 10;
  for (int y = 0; y < MAP; ++y) {
    std::string row = "  ";
    for (int x = 0; x < MAP; ++x) {
      double norm = (heights[y][x] - hmin) / (hmax - hmin + 1e-9);
      int idx = static_cast<int>(norm * (shade_count - 1));
      if (idx < 0) idx = 0;
      if (idx >= shade_count) idx = shade_count - 1;
      row += shades[idx];
      row += ' ';
    }
    w << row;
  }
  w << "";

  w << ("!i Height range: [" + dtos(hmin, 3) + ", " + dtos(hmax, 3) + "]");
  w << "- ` ` = deep water";
  w << "- `.` = shallow water";
  w << "- `=` = plains";
  w << "- `#` = hills";
  w << "- `@` = mountains";
  w << "";

  // ── Random coordinate generator — tree placement ──────────────────
  w << "## Random Coordinate Generator — Tree Placement";

  libcpp::math::IVector2 map_min(0, 0);
  libcpp::math::IVector2 map_max(MAP - 1, MAP - 1);
  libcpp::math::Random2DCoordinateGenerator gen(1337, map_min, map_max);

  w << ("!i Bounding box: (0,0) to ("
      + libcpp::str::to_string(MAP - 1) + ","
      + libcpp::str::to_string(MAP - 1) + ")");
  w << ("!i Total possible positions: "
      + libcpp::str::to_string(static_cast<int>(gen.remaining())));
  w << "";

  int tree_count = 8;
  w << "| # | Position | Terrain Height |";
  w << "| :---: | :---: | ---: |";
  for (int i = 0; i < tree_count; ++i) {
    libcpp::math::IVector2 pos = gen();
    double h = heights[pos.y][pos.x];
    std::ostringstream p;
    p << pos;
    w << ("| " + libcpp::str::to_string(i + 1)
        + " | " + p.str()
        + " | " + dtos(h, 3) + " |");
  }
  w << "";

  w << ("!v " + libcpp::str::to_string(tree_count) + " trees placed, "
      + libcpp::str::to_string(static_cast<int>(gen.remaining()))
      + " positions remaining");
  w << "";

  // ── Summary callout ───────────────────────────────────────────────
  w << ">![tip] Math Module Summary";
  w << "> `IVector2` — 2D integer vector with +, -, *, /, dot, cross, length";
  w << "> `IVector3` — 3D integer vector with full operator set";
  w << "> `PerlinNoise2D` — classic gradient noise with octave layering";
  w << "> `Random2DCoordinateGenerator` — unique random positions in a rect";
  w << "";

  w << "---";
  w << "!v World generation complete — terrain + trees placed.";
  return 0;
}
