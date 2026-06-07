#include "netlist_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cctype>

// ============================================================
//  Header parsing
// ============================================================

std::string NetlistParser::parse_header(std::ifstream& file) {
    if (!file.is_open()) return "";

    std::streampos original_pos = file.tellg();
    std::string    first_line;

    if (!std::getline(file, first_line)) return "";

    size_t start = first_line.find_first_not_of(" \t\r\n");
    if (start == std::string::npos || first_line[start] != '*') {
        file.seekg(original_pos);  // not a header — rewind
        return "";
    }

    size_t content_start = first_line.find_first_not_of(" \t*", start);
    if (content_start == std::string::npos) return "";

    std::string title = first_line.substr(content_start);
    size_t end = title.find_last_not_of(" \t\r\n");
    return (end != std::string::npos) ? title.substr(0, end + 1) : title;
}

// ============================================================
//  Line parsing
// ============================================================

bool NetlistParser::parse_line(const std::string& line, ComponentDescriptor& out) {
    out = ComponentDescriptor();

    size_t start = line.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return false;
    if (should_skip(line, start))   return false;

    std::istringstream iss(line.substr(start));
    std::string first_token;
    if (!(iss >> first_token))        return false;
    if (should_skip(first_token, 0))  return false;

    if (first_token[0] == '.') {
        return parse_directive(iss, first_token, out);
    }

    out.id   = first_token;
    out.type = static_cast<char>(std::toupper(
                   static_cast<unsigned char>(first_token[0])));

    if (!(iss >> out.node1 >> out.node2)) {
        throw std::runtime_error("Missing nodes for component '" + out.id + "'");
    }

    parse_values(iss, out, line);

    if (out.type != 'V' && out.positional.empty() && out.keyed.empty()) {
        throw std::runtime_error(
            "Missing value for component '" + out.id + "'"
        );
    }

    return true;
}

// ============================================================
//  Helpers
// ============================================================

bool NetlistParser::should_skip(const std::string& line, size_t start) {
    if (line.empty() || start >= line.size()) return true;
    char c = line[start];
    if (c == '*') return true;
    if (c == '/' && start + 1 < line.size() && line[start + 1] == '/') return true;
    return false;
}

bool NetlistParser::is_inline_comment(const std::string& token) {
    return token.size() >= 2 && token[0] == '/' && token[1] == '/';
}

std::string NetlistParser::to_upper(const std::string& token) {
    std::string result = token;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

double NetlistParser::parse_value(const std::string& token,
                                  const std::string& line) {
    try {
        return std::stod(token);
    } catch (const std::exception&) {
        throw std::runtime_error(
            "Cannot parse '" + token + "' as a number in: " + line
        );
    }
}

bool NetlistParser::parse_directive(std::istringstream& iss,
                                     const std::string&  first_token,
                                     ComponentDescriptor& out) {
    out.is_directive = true;
    out.raw_tokens.push_back(first_token);

    std::string token;
    while (iss >> token) {
        if (is_inline_comment(token)) break;
        out.raw_tokens.push_back(token);
    }
    return true;
}

void NetlistParser::parse_values(std::istringstream& iss,
                                  ComponentDescriptor& out,
                                  const std::string&  line) {
    std::string token;
    while (iss >> token) {
        if (is_inline_comment(token)) break;

        std::string upper = to_upper(token);

        if (upper == "DC" || upper == "AC") {
            std::string val_token;
            if (!(iss >> val_token)) {
                throw std::runtime_error(
                    "Missing value after keyword '" + token + "' in: " + line
                );
            }
            out.keyed[upper] = parse_value(val_token, line);
        } else {
            out.positional.push_back(parse_value(token, line));
        }
    }
}