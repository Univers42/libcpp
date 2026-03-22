#include "../Database.hpp"
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Usage: demo_csv <csv-path>\n";
		return 1;
	}
	std::string path = argv[1];

	try {
		Database db;
		// load (autoIncrementId enabled by default)
		db.loadFromCsv(path, true);

		std::cout << "\n--- DEFAULT ---\n";
		std::cout << db.render();

		// Elegant heavy border + header color + footer (fits)
		{
			RenderConfig cfg = RenderConfig::elegant();
			cfg.boxChars = Unicode::BoxChars::heavy();
			cfg.headerStyle.foreground = Style::Color::BrightMagenta();
			cfg.headerStyle.bold = true;
			cfg.borderStyle.foreground = Style::Color::BrightBlue();
			cfg.showFooter = true;
			{
				std::ostringstream ss;
				ss << "Loaded " << db.count() << " rows × " << db.table().columnCount() << " columns";
				cfg.footerText = ss.str();
			}
			cfg.footerStyle.foreground = Style::Color::BrightYellow();
			std::cout << "\n--- ELEGANT + HEAVY BORDER + FOOTER ---\n";
			std::cout << db.render(cfg);
		}

		// Body background + body foreground + even/odd stripe + value coloring
		{
			RenderConfig cfg;
			cfg.boxChars = Unicode::BoxChars::doubleLine();
			cfg.borderStyle.foreground = Style::Color::BrightWhite();
			cfg.cellStyle.foreground = Style::Color::BrightBlack();

			// body-level foreground and background: white background + subtle green stripe
			cfg.color_body = Style::Color::BrightBlack();      // body text color
			cfg.body_background = Style::Color(47);            // ANSI 47 = white background
			cfg.even_background = Style::Color(102);           // ANSI 102 = bright green background (subtle stripe)
			cfg.odd_background  = Style::Color(47);            // keep odd rows white

			// value-based coloring for booleans (keys must be lowercase)
			Style::CellStyle trueStyle;
			trueStyle.foreground = Style::Color::BrightGreen();
			trueStyle.bold = true;
			cfg.value_styles["true"] = trueStyle;

			Style::CellStyle falseStyle;
			falseStyle.foreground = Style::Color::BrightRed();
			falseStyle.bold = true;
			cfg.value_styles["false"] = falseStyle;

			cfg.padding = 1;
			std::cout << "\n--- BODY BACKGROUND + STRIPES + VALUE COLORS ---\n";
			std::cout << db.render(cfg);
		}

		// Per-column color overrides (Score highlighted)
		{
			RenderConfig cfg = RenderConfig::minimal();
			cfg.boxChars = Unicode::BoxChars::doubleLine();
			cfg.borderStyle.foreground = Style::Color::Default();
			cfg.color_column["Score"] = Style::Color::BrightYellow();
			cfg.color_column["Name"]  = Style::Color::BrightCyan();
			cfg.padding = 1;
			std::cout << "\n--- PER-COLUMN COLORS (Score, Name) ---\n";
			std::cout << db.render(cfg);
		}

		// Very long footer printed outside table
		{
			RenderConfig cfg = RenderConfig::elegant();
			cfg.boxChars = Unicode::BoxChars::doubleLine();
			cfg.borderStyle.foreground = Style::Color::BrightCyan();
			cfg.showFooter = true;
			std::ostringstream ss;
			ss << "Loaded " << db.count() << " rows × " << db.table().columnCount()
			   << " columns — this is a very long footer that should be printed outside the table to avoid misalignment and demonstrate footer placement behavior";
			cfg.footerText = ss.str();
			cfg.footerStyle.foreground = Style::Color::BrightGreen();
			std::cout << "\n--- LONG FOOTER (outside table) ---\n";
			std::cout << db.render(cfg);
		}

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 2;
	}

	return 0;
}