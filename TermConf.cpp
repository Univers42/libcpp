#include "TermConf.hpp"

/* ═══════════════════════════════════════════════════════════
 *  TermConf — default constructor (the "default" preset)
 * ═══════════════════════════════════════════════════════════ */

TermConf::TermConf()
    :
    /* ── global ────────────────────────────────────── */
    globalWidth(60),

    /* ── heading spacing ───────────────────────────── */
    h1SpaceBefore(1), h1SpaceAfter(1),
    h2SpaceBefore(1), h2SpaceAfter(0),
    h3SpaceBefore(0), h3SpaceAfter(0),

    /* ── log spacing ───────────────────────────────── */
    logSpaceBefore(0), logSpaceAfter(0),

    /* ── block spacing ─────────────────────────────── */
    sepSpaceBefore(0), sepSpaceAfter(0),
    quoteSpaceBefore(0), quoteSpaceAfter(0),
    sectionSpaceBefore(1), sectionSpaceAfter(1),
    textSpaceBefore(0), textSpaceAfter(0),

    /* ── heading colors ────────────────────────────── */
    h1Fg(220, 160, 255),     h1Border(160, 100, 220),
    h2Fg(130, 200, 255),     h2Border( 70, 130, 200),
    h3Fg(180, 220, 180),     h3Border( 80, 140,  80),

    /* ── heading fonts ─────────────────────────────── */
    h1Font(TermStyle::BOLD),
    h2Font(TermStyle::BOLD),
    h3Font(TermStyle::BOLD | TermStyle::ITALIC),

    /* ── heading glyphs ────────────────────────────── */
    h1Glyph(Glyph::DIAMOND),
    h2Glyph(Glyph::TRIANGLE),
    h3Glyph(Glyph::BULLET),

    /* ── heading separators ────────────────────────── */
    h1Sep(Glyph::DOUBLE),
    h2Sep(Glyph::HDASH_HVY),
    h3Sep(Glyph::DOTTED),

    /* ── heading alignment ─────────────────────────── */
    h1Align(TermStyle::CENTER),
    h2Align(TermStyle::LEFT),
    h3Align(TermStyle::LEFT),

    /* ── log colors ────────────────────────────────── */
    infoFg(  0, 200, 255),
    warnFg(255, 200,   0),
    errorFg(255,  60,  60),
    successFg(  0, 230, 120),

    /* ── log glyphs ────────────────────────────────── */
    infoGlyph(Glyph::INFO),
    warnGlyph(Glyph::WARN),
    errorGlyph(Glyph::CROSS),
    successGlyph(Glyph::CHECK),

    /* ── quote ─────────────────────────────────────── */
    quoteFg(180, 180, 180),
    quoteGlyph(Glyph::VBAR),

    /* ── separator ─────────────────────────────────── */
    sepBorder(100, 100, 100),
    sepStr(Glyph::HDASH),

    /* ── text ──────────────────────────────────────── */
    textFg(210, 210, 210),

    /* ── list ──────────────────────────────────────── */
    bulletFg(200, 210, 220),
    bulletGlyph(Glyph::BULLET),
    olFg(200, 210, 220),

    /* ── callout ───────────────────────────────────── */
    calloutFg(180, 200, 220),
    calloutBg( 20,  30,  45),
    calloutBorder( 60,  80, 110),
    calloutGlyph(Glyph::VBAR_THIN),
    calloutBodyGlyph(Glyph::VBAR_THIN),
    calloutHasBg(true),

    /* ── h1 background ─────────────────────────────── */
    h1Bg( 50,  30,  80),
    h1HasBg(true),
    h1PadV(1)
{}

TermConf::~TermConf() {}

/* ═══════════════════════════════════════════════════════════
 *  apply — push this config into a TermStyle instance
 * ═══════════════════════════════════════════════════════════ */

void TermConf::apply(TermStyle& ts) const {
    /* ── h1 ────────────────────────────────────────── */
    ts.h1Style().fg          = h1Fg;
    ts.h1Style().bg          = h1Bg;
    ts.h1Style().border      = h1Border;
    ts.h1Style().font        = h1Font;
    ts.h1Style().align       = h1Align;
    ts.h1Style().width       = globalWidth;
    ts.h1Style().glyph       = h1Glyph;
    ts.h1Style().sepStr      = h1Sep;
    ts.h1Style().spaceBefore = h1SpaceBefore;
    ts.h1Style().spaceAfter  = h1SpaceAfter;
    ts.h1Style().hasBg       = h1HasBg;
    ts.h1Style().padV        = h1PadV;

    /* ── h2 ────────────────────────────────────────── */
    ts.h2Style().fg          = h2Fg;
    ts.h2Style().border      = h2Border;
    ts.h2Style().font        = h2Font;
    ts.h2Style().align       = h2Align;
    ts.h2Style().width       = globalWidth;
    ts.h2Style().glyph       = h2Glyph;
    ts.h2Style().sepStr      = h2Sep;
    ts.h2Style().spaceBefore = h2SpaceBefore;
    ts.h2Style().spaceAfter  = h2SpaceAfter;

    /* ── h3 ────────────────────────────────────────── */
    ts.h3Style().fg          = h3Fg;
    ts.h3Style().border      = h3Border;
    ts.h3Style().font        = h3Font;
    ts.h3Style().align       = h3Align;
    ts.h3Style().width       = globalWidth;
    ts.h3Style().glyph       = h3Glyph;
    ts.h3Style().sepStr      = h3Sep;
    ts.h3Style().spaceBefore = h3SpaceBefore;
    ts.h3Style().spaceAfter  = h3SpaceAfter;

    /* ── text ──────────────────────────────────────── */
    ts.textStyle().fg          = textFg;
    ts.textStyle().width       = globalWidth;
    ts.textStyle().spaceBefore = textSpaceBefore;
    ts.textStyle().spaceAfter  = textSpaceAfter;

    /* ── info ──────────────────────────────────────── */
    ts.infoStyle().fg          = infoFg;
    ts.infoStyle().glyph       = infoGlyph;
    ts.infoStyle().width       = globalWidth;
    ts.infoStyle().spaceBefore = logSpaceBefore;
    ts.infoStyle().spaceAfter  = logSpaceAfter;

    /* ── warn ──────────────────────────────────────── */
    ts.warnStyle().fg          = warnFg;
    ts.warnStyle().glyph       = warnGlyph;
    ts.warnStyle().width       = globalWidth;
    ts.warnStyle().spaceBefore = logSpaceBefore;
    ts.warnStyle().spaceAfter  = logSpaceAfter;

    /* ── error ─────────────────────────────────────── */
    ts.errorStyle().fg          = errorFg;
    ts.errorStyle().glyph       = errorGlyph;
    ts.errorStyle().width       = globalWidth;
    ts.errorStyle().spaceBefore = logSpaceBefore;
    ts.errorStyle().spaceAfter  = logSpaceAfter;

    /* ── success ───────────────────────────────────── */
    ts.successStyle().fg          = successFg;
    ts.successStyle().glyph       = successGlyph;
    ts.successStyle().width       = globalWidth;
    ts.successStyle().spaceBefore = logSpaceBefore;
    ts.successStyle().spaceAfter  = logSpaceAfter;

    /* ── quote ─────────────────────────────────────── */
    ts.quoteStyle().fg          = quoteFg;
    ts.quoteStyle().glyph       = quoteGlyph;
    ts.quoteStyle().width       = globalWidth;
    ts.quoteStyle().spaceBefore = quoteSpaceBefore;
    ts.quoteStyle().spaceAfter  = quoteSpaceAfter;

    /* ── separator ─────────────────────────────────── */
    ts.sepStyle().border      = sepBorder;
    ts.sepStyle().sepStr      = sepStr;
    ts.sepStyle().width       = globalWidth;
    ts.sepStyle().spaceBefore = sepSpaceBefore;
    ts.sepStyle().spaceAfter  = sepSpaceAfter;

    /* ── bullet list ───────────────────────────────── */
    ts.bulletStyle().fg    = bulletFg;
    ts.bulletStyle().glyph = bulletGlyph;
    ts.bulletStyle().width = globalWidth;

    /* ── ordered list ──────────────────────────────── */
    ts.olStyle().fg    = olFg;
    ts.olStyle().width = globalWidth;

    /* ── callout ───────────────────────────────────── */
    ts.calloutStyle().fg        = calloutFg;
    ts.calloutStyle().bg        = calloutBg;
    ts.calloutStyle().border    = calloutBorder;
    ts.calloutStyle().glyph     = calloutGlyph;
    ts.calloutStyle().bodyGlyph = calloutBodyGlyph;
    ts.calloutStyle().hasBg     = calloutHasBg;
    ts.calloutStyle().width     = globalWidth;
}

/* ═══════════════════════════════════════════════════════════
 *  Presets
 * ═══════════════════════════════════════════════════════════ */

void TermConf::applyDefault(TermStyle& ts) {
    TermConf conf;
    conf.apply(ts);
}

void TermConf::applyMinimal(TermStyle& ts) {
    TermConf conf;

    /* minimal: narrow, no extra spacing, simple glyphs */
    conf.globalWidth    = 50;
    conf.h1SpaceBefore  = 1;
    conf.h1SpaceAfter   = 0;
    conf.h2SpaceBefore  = 0;
    conf.h2SpaceAfter   = 0;
    conf.sectionSpaceBefore = 0;
    conf.sectionSpaceAfter  = 0;

    conf.h1Font  = TermStyle::BOLD;
    conf.h2Font  = TermStyle::BOLD;
    conf.h3Font  = TermStyle::NONE;

    conf.h1Glyph = Glyph::TRIANGLE;
    conf.h2Glyph = Glyph::BULLET;
    conf.h3Glyph = "";

    conf.h1Sep   = Glyph::HDASH;
    conf.h2Sep   = Glyph::HDASH;
    conf.h3Sep   = "";

    conf.h1Fg     = Srgb(200, 200, 200);
    conf.h1Border = Srgb(120, 120, 120);
    conf.h2Fg     = Srgb(180, 180, 180);
    conf.h2Border = Srgb(100, 100, 100);

    conf.apply(ts);
}

void TermConf::applyCompact(TermStyle& ts) {
    TermConf conf;

    /* compact: zero spacing everywhere */
    conf.globalWidth        = 60;
    conf.h1SpaceBefore      = 0;
    conf.h1SpaceAfter       = 0;
    conf.h2SpaceBefore      = 0;
    conf.h2SpaceAfter       = 0;
    conf.h3SpaceBefore      = 0;
    conf.h3SpaceAfter       = 0;
    conf.logSpaceBefore     = 0;
    conf.logSpaceAfter      = 0;
    conf.sepSpaceBefore     = 0;
    conf.sepSpaceAfter      = 0;
    conf.sectionSpaceBefore = 0;
    conf.sectionSpaceAfter  = 0;

    conf.apply(ts);
}

void TermConf::applyWide(TermStyle& ts) {
    TermConf conf;

    /* wide: 80 cols, generous spacing */
    conf.globalWidth        = 80;
    conf.h1SpaceBefore      = 2;
    conf.h1SpaceAfter       = 1;
    conf.h2SpaceBefore      = 1;
    conf.h2SpaceAfter       = 1;
    conf.h3SpaceBefore      = 1;
    conf.h3SpaceAfter       = 0;
    conf.sectionSpaceBefore = 1;
    conf.sectionSpaceAfter  = 1;

    conf.h1Fg     = Srgb(255, 200, 100);
    conf.h1Border = Srgb(200, 150, 50);
    conf.h1Sep    = Glyph::HDASH_HVY;
    conf.h2Sep    = Glyph::DOUBLE;

    conf.apply(ts);
}
