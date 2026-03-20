#include "../Database.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
// add readline for better prompt handling
#include <readline/readline.h>
#include <readline/history.h>

static std::string trim(const std::string &s) {
	std::string r = s;
	while (!r.empty() && (r[0] == ' ' || r[0] == '\t')) r.erase(r.begin());
	while (!r.empty() && (r[r.size()-1] == ' ' || r[r.size()-1] == '\t' || r[r.size()-1]=='\r' || r[r.size()-1]=='\n')) r.erase(r.size()-1);
	return r;
}

static std::vector<std::string> split_ws(const std::string &s) {
	std::vector<std::string> out;
	std::istringstream iss(s);
	std::string tok;
	while (iss >> tok) out.push_back(tok);
	return out;
}

static std::string basename_no_ext(const std::string &path) {
	size_t pos = path.find_last_of("/\\");
	std::string b = (pos==std::string::npos) ? path : path.substr(pos+1);
	size_t dot = b.find_last_of('.');
	if (dot == std::string::npos) return b;
	return b.substr(0,dot);
}

static void load_csv_into(std::map<std::string, Database> &dbs, const std::string &path) {
	struct stat st;
	if (stat(path.c_str(), &st) != 0) {
		std::cerr << "Cannot stat: " << path << "\n";
		return;
	}
	std::string name = basename_no_ext(path);
	Database db;
	try {
		db.loadFromCsv(path, true);
		dbs[name] = db;
		std::cout << "Loaded '" << name << "' from " << path << "\n";
	} catch (const std::exception &e) {
		std::cerr << "Load failed: " << e.what() << "\n";
	}
}

static void load_all_csvs(std::map<std::string, Database> &dbs, const std::string &dirpath) {
	DIR *d = opendir(dirpath.c_str());
	if (!d) {
		std::cerr << "Cannot open directory: " << dirpath << "\n";
		return;
	}
	struct dirent *ent;
	while ((ent = readdir(d)) != NULL) {
		std::string n(ent->d_name);
		if (n.size() > 4 && n.substr(n.size()-4) == ".csv") {
			std::string full = dirpath + "/" + n;
			load_csv_into(dbs, full);
		}
	}
	closedir(d);
}

static void show_databases(const std::map<std::string, Database> &dbs) {
	if (dbs.empty()) { std::cout << "(no databases loaded)\n"; return; }
	for (std::map<std::string, Database>::const_iterator it = dbs.begin(); it != dbs.end(); ++it) {
		std::cout << it->first << "\n";
	}
}

static void show_tables(const std::map<std::string, Database> &dbs, const std::string &name) {
	std::map<std::string, Database>::const_iterator it = dbs.find(name);
	if (it == dbs.end()) { std::cerr << "No such database: " << name << "\n"; return; }
	const Table &t = it->second.table();
	std::cout << "Table columns for " << name << ":\n";
	const std::vector<Column> &cols = t.columns();
	for (size_t i = 0; i < cols.size(); ++i) std::cout << " - " << cols[i].name() << "\n";
}

static Table filter_table_by_where(const Table &src, const std::string &where_col, const std::string &where_val) {
	Table out;
	// copy columns
	const std::vector<Column> &cols = src.columns();
	for (size_t i = 0; i < cols.size(); ++i) out.addColumn(cols[i]);
	// add rows that match
	const std::vector<Row> &rows = src.rows();
	for (size_t r = 0; r < rows.size(); ++r) {
		std::string v = rows[r].getValue(where_col);
		if (v == where_val) out.addRow(rows[r]);
	}
	return out;
}

static void cmd_select(const std::map<std::string, Database> &dbs, const std::string &cmd) {
	// naive parse: SELECT <cols> FROM <db> [WHERE col=value]
	std::string lower = cmd;
	std::string s = cmd;
	// find FROM
	size_t from = s.find(" FROM ");
	if (from == std::string::npos) { std::cerr << "Malformed SELECT, missing FROM\n"; return; }
	std::string cols_part = trim(s.substr(6, from-6));
	size_t where_pos = s.find(" WHERE ", from);
	std::string from_part, where_part;
	if (where_pos == std::string::npos) {
		from_part = trim(s.substr(from+6));
	} else {
		from_part = trim(s.substr(from+6, where_pos - (from+6)));
		where_part = trim(s.substr(where_pos+7));
	}
	std::string dbname = from_part;
	std::map<std::string, Database>::const_iterator it = dbs.find(dbname);
	if (it == dbs.end()) { std::cerr << "No such database: " << dbname << "\n"; return; }
	const Table &tbl = it->second.table();
	Table tmp;
	// handle WHERE
	if (!where_part.empty()) {
		// expect col=value
		size_t eq = where_part.find('=');
		if (eq == std::string::npos) { std::cerr << "Malformed WHERE, expect col=value\n"; return; }
		std::string col = trim(where_part.substr(0,eq));
		std::string val = trim(where_part.substr(eq+1));
		// remove surrounding quotes if any
		if (val.size() >=2 && ((val[0]=='"' && val[val.size()-1]=='"') || (val[0]=='\'' && val[val.size()-1]=='\'')))
			val = val.substr(1,val.size()-2);
		tmp = filter_table_by_where(tbl, col, val);
	} else {
		// copy whole table
		tmp = tbl;
	}
	// if cols_part == "*" render whole tmp; else build projection
	if (cols_part == "*" || cols_part == " *") {
		RenderConfig cfg = RenderConfig::elegant();
		TableRenderer tr(cfg);
		std::cout << tr.render(tmp);
	} else {
		// build projection table with selected column names
		std::vector<std::string> wanted;
		std::istringstream iss(cols_part);
		std::string token;
		while (std::getline(iss, token, ',')) wanted.push_back(trim(token));
		Table out;
		// add columns with same type/align where matched
		const std::vector<Column> &allcols = tmp.columns();
		for (size_t i = 0; i < wanted.size(); ++i) {
			for (size_t j = 0; j < allcols.size(); ++j) {
				if (allcols[j].name() == wanted[i]) { out.addColumn(allcols[j]); break; }
			}
		}
		// add rows projected
		const std::vector<Row> &rows = tmp.rows();
		for (size_t r = 0; r < rows.size(); ++r) {
			Row nr;
			for (size_t c = 0; c < wanted.size(); ++c) {
				nr.setValue(wanted[c], rows[r].getValue(wanted[c]));
			}
			out.addRow(nr);
		}
		RenderConfig cfg = RenderConfig::elegant();
		TableRenderer tr(cfg);
		std::cout << tr.render(out);
	}
}

static void cmd_delete_where(std::map<std::string, Database> &dbs, const std::string &cmd) {
	// format: DELETE FROM <db> WHERE col=value
	size_t pos = cmd.find("DELETE FROM ");
	if (pos != 0) { std::cerr << "Malformed DELETE\n"; return; }
	size_t where = cmd.find(" WHERE ");
	if (where == std::string::npos) { std::cerr << "DELETE requires WHERE\n"; return; }
	std::string dbname = trim(cmd.substr(12, where-12));
	std::string cond = trim(cmd.substr(where+7));
	size_t eq = cond.find('=');
	if (eq == std::string::npos) { std::cerr << "Malformed WHERE\n"; return; }
	std::string col = trim(cond.substr(0,eq));
	std::string val = trim(cond.substr(eq+1));
	if (val.size()>=2 && ((val[0]=='"' && val[val.size()-1]=='"') || (val[0]=='\'' && val[val.size()-1]=='\''))) val = val.substr(1,val.size()-2);
	std::map<std::string, Database>::iterator it = dbs.find(dbname);
	if (it == dbs.end()) { std::cerr << "No such database: " << dbname << "\n"; return; }
	Table &t = it->second.table();
	// remove rows where column equals val
	std::vector<Row> &rows = t.rows();
	std::vector<Row> keep;
	for (size_t r = 0; r < rows.size(); ++r) {
		if (rows[r].getValue(col) != val) keep.push_back(rows[r]);
	}
	rows.swap(keep);
	std::cout << "Deleted matching rows from " << dbname << "\n";
}

static void cmd_add_column(std::map<std::string, Database> &dbs, const std::string &cmd) {
	// ADD COLUMN <db> <colname>
	std::vector<std::string> parts = split_ws(cmd);
	if (parts.size() < 4) { std::cerr << "Usage: ADD COLUMN <db> <colname>\n"; return; }
	std::string db = parts[2];
	std::string colname = parts[3];
	std::map<std::string, Database>::iterator it = dbs.find(db);
	if (it == dbs.end()) { std::cerr << "No such database: " << db << "\n"; return; }
	it->second.table().addColumn(Column(colname));
	// initialize empty value for existing rows
	std::vector<Row> &rows = it->second.table().rows();
	for (size_t r = 0; r < rows.size(); ++r) rows[r].setValue(colname, "");
	std::cout << "Added column " << colname << " to " << db << "\n";
}

static void cmd_delete_column(std::map<std::string, Database> &dbs, const std::string &cmd) {
	// DELETE COLUMN <db> <colname>
	std::vector<std::string> parts = split_ws(cmd);
	if (parts.size() < 4) { std::cerr << "Usage: DELETE COLUMN <db> <colname>\n"; return; }
	std::string db = parts[2];
	std::string colname = parts[3];
	std::map<std::string, Database>::iterator it = dbs.find(db);
	if (it == dbs.end()) { std::cerr << "No such database: " << db << "\n"; return; }
	Table &t = it->second.table();
	// remove column from columns vector
	std::vector<Column> newcols;
	const std::vector<Column> &cols = t.columns();
	for (size_t i = 0; i < cols.size(); ++i) if (cols[i].name() != colname) newcols.push_back(cols[i]);
	t.columns() = newcols;
	// remove values from rows
	std::vector<Row> &rows = t.rows();
	for (size_t r = 0; r < rows.size(); ++r) {
		std::map<std::string,std::string> newdata;
		const std::map<std::string,std::string> &old = rows[r].data();
		for (std::map<std::string,std::string>::const_iterator it2 = old.begin(); it2 != old.end(); ++it2) {
			if (it2->first != colname) newdata[it2->first] = it2->second;
		}
		rows[r] = Row(); // reset
		// copy back
		for (std::map<std::string,std::string>::const_iterator it3 = newdata.begin(); it3 != newdata.end(); ++it3) rows[r].setValue(it3->first, it3->second);
	}
	std::cout << "Deleted column " << colname << " from " << db << "\n";
}

static void cmd_add_row(std::map<std::string, Database> &dbs, const std::string &cmd) {
	// ADD ROW <db>  -> prompt user to fill columns
	std::vector<std::string> parts = split_ws(cmd);
	if (parts.size() < 3) { std::cerr << "Usage: ADD ROW <db>\n"; return; }
	std::string db = parts[2];
	std::map<std::string, Database>::iterator it = dbs.find(db);
	if (it == dbs.end()) { std::cerr << "No such database: " << db << "\n"; return; }
	Table &t = it->second.table();
	Row r;
	const std::vector<Column> &cols = t.columns();
	for (size_t i = 0; i < cols.size(); ++i) {
		// use readline for each field
		std::string prompt = cols[i].name() + ": ";
		char *res = readline(prompt.c_str());
		std::string val;
		if (res) {
			val = res;
			if (val.size()) add_history(res);
			free(res);
		} else {
			val = "";
		}
		r.setValue(cols[i].name(), val);
	}
	t.addRow(r);
	std::cout << "Added row to " << db << "\n";
}

static void cmd_save(std::map<std::string, Database> &dbs, const std::string &cmd) {
	// SAVE <db> <path>
	std::vector<std::string> parts = split_ws(cmd);
	if (parts.size() < 3) { std::cerr << "Usage: SAVE <db> <path>\n"; return; }
	std::string db = parts[1];
	std::string path = (parts.size() >=3 ? parts[2] : std::string());
	// allow SAVE name path
	if (parts.size() >= 4) { db = parts[1]; path = parts[2]; }
	std::map<std::string, Database>::iterator it = dbs.find(db);
	if (it == dbs.end()) { std::cerr << "No such database: " << db << "\n"; return; }
	if (path.empty()) { std::cerr << "Missing path\n"; return; }
	// export CSV
	std::ofstream ofs(path.c_str());
	if (!ofs.is_open()) { std::cerr << "Cannot open " << path << " for writing\n"; return; }
	const Table &t = it->second.table();
	// header
	const std::vector<Column> &cols = t.columns();
	for (size_t i = 0; i < cols.size(); ++i) {
		if (i) ofs << ",";
		ofs << cols[i].name();
	}
	ofs << "\n";
	// rows
	const std::vector<Row> &rows = t.rows();
	for (size_t r = 0; r < rows.size(); ++r) {
		for (size_t c = 0; c < cols.size(); ++c) {
			if (c) ofs << ",";
			std::string v = rows[r].getValue(cols[c].name());
			// simple quoting
			if (v.find(',') != std::string::npos || v.find('"') != std::string::npos) {
				std::string v2 = v;
				// escape quotes
				std::string esc;
				for (size_t k = 0; k < v2.size(); ++k) {
					if (v2[k] == '"') esc += "\"\"";
					else esc += v2[k];
				}
				ofs << "\"" << esc << "\"";
			} else {
				ofs << v;
			}
		}
		ofs << "\n";
	}
	ofs.close();
	std::cout << "Saved " << db << " to " << path << "\n";
}

// new: try several candidate locations and return the first existing directory
static std::string find_archives_dir() {
	std::vector<std::string> candidates;
	candidates.push_back("C:\\Users\\dylan\\Shared\\picine_cpp\\archives_test");
	candidates.push_back("./archives_test");
	candidates.push_back("../archives_test");
	candidates.push_back("../../archives_test");
	candidates.push_back("../archives/archives_test");
	candidates.push_back("/mnt/c/Users/dylan/Shared/picine_cpp/archives_test"); // WSL path fallback
	struct stat st;
	for (size_t i = 0; i < candidates.size(); ++i) {
		const std::string &p = candidates[i];
		if (stat(p.c_str(), &st) == 0 && (st.st_mode & S_IFDIR)) {
			return p;
		}
	}
	return std::string();
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	std::map<std::string, Database> dbs;

	// discover archives folder from several likely locations (Windows, relative, WSL)
	std::string archives = find_archives_dir();
	if (archives.empty()) {
		std::cerr << "No archives directory found (tried several locations). Use LOAD <path> to load CSVs.\n";
	} else {
		std::cout << "Using archives directory: " << archives << "\n";
		load_all_csvs(dbs, archives);
	}

	std::cout << "CSV-DB REPL. Type HELP for commands.\n";
	while (true) {
		char *rl = readline("db> ");
		if (!rl) break;
		std::string line = rl;
		free(rl);
		// skip empty
		line = trim(line);
		if (line.empty()) continue;
		if (line.size()) add_history(line.c_str());

		std::string up = line;
		// uppercase command keyword for simple dispatch
		std::string cmdKey = line;
		// find first word
		std::istringstream iss(line);
		std::string first;
		iss >> first;
		std::string first_up = first;
		for (size_t i = 0; i < first_up.size(); ++i) first_up[i] = toupper((unsigned char)first_up[i]);
		if (first_up == "QUIT" || first_up == "EXIT") break;
		if (first_up == "HELP") {
			std::cout << "Commands: LOAD <path>, LOADALL, SHOW DATABASES, SHOW TABLES <db>, SELECT ..., DELETE FROM <db> WHERE col=val,\n"
					  << "ADD COLUMN <db> <col>, DELETE COLUMN <db> <col>, ADD ROW <db>, SAVE <db> <path>, QUIT\n";
			continue;
		}
		// dispatch
		std::string upperLine = line;
		for (size_t i=0;i<upperLine.size();++i) upperLine[i] = toupper((unsigned char)upperLine[i]);
		if (upperLine.find("LOADALL") == 0) {
			load_all_csvs(dbs, archives);
			continue;
		} else if (upperLine.find("LOAD ") == 0) {
			std::string path = trim(line.substr(5));
			load_csv_into(dbs, path);
			continue;
		} else if (upperLine.find("SHOW DATABASES") == 0) {
			show_databases(dbs);
			continue;
		} else if (upperLine.find("SHOW TABLES") == 0) {
			std::string name = trim(line.substr(11));
			show_tables(dbs, name);
			continue;
		} else if (upperLine.find("SELECT ") == 0) {
			cmd_select(dbs, line);
			continue;
		} else if (upperLine.find("DELETE FROM ") == 0) {
			cmd_delete_where(dbs, line);
			continue;
		} else if (upperLine.find("ADD COLUMN ") == 0) {
			cmd_add_column(dbs, line);
			continue;
		} else if (upperLine.find("DELETE COLUMN ") == 0) {
			cmd_delete_column(dbs, line);
			continue;
		} else if (upperLine.find("ADD ROW ") == 0) {
			cmd_add_row(dbs, line);
			continue;
		} else if (upperLine.find("SAVE ") == 0) {
			cmd_save(dbs, line);
			continue;
		} else {
			std::cout << "Unknown or unsupported command. Type HELP.\n";
		}
	}

	std::cout << "Bye.\n";
	return 0;
}
