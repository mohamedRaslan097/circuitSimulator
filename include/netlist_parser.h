#ifndef NETLIST_PARSER_H
#define NETLIST_PARSER_H

#include <string>
#include <fstream>
#include <sstream>
#include "component_descriptor.h"

/**
 * @class NetlistParser
 * @brief Stateless static utility for parsing SPICE-like netlist files.
 * 
 * Extracts circuit names from headers and tokenizes line declarations into
 * ComponentDescriptor structures.
 */
class NetlistParser {
public:
    /**
     * @brief Parses the first line/header comment if it starts with '*'
     * @param file Reference to an open input file stream.
     * @return The parsed circuit name or empty string if not a name comment.
     */
    static std::string parse_header(std::ifstream& file);

    /**
     * @brief Parses a single netlist line into a ComponentDescriptor.
     * @param line Raw line text from file.
     * @param out Reference to output descriptor structure.
     * @return true if parsed as component or directive, false if empty or comment.
     * @throws std::runtime_error on malformed syntax.
     */
    static bool parse_line(const std::string& line, ComponentDescriptor& out);

private:
    static bool should_skip(const std::string& line, size_t start);
    static bool is_inline_comment(const std::string& token);
    static std::string to_upper(const std::string& token);
    static double parse_value(const std::string& token, const std::string& line);
    static bool parse_directive(std::istringstream& iss,
                                const std::string& first_token,
                                ComponentDescriptor& out);
    static void parse_values(std::istringstream& iss,
                             ComponentDescriptor& out,
                             const std::string& line);
};

#endif
