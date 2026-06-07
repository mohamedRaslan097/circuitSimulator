#ifndef COMPONENT_DESCRIPTOR_H
#define COMPONENT_DESCRIPTOR_H

#include <string>
#include <vector>
#include <map>

class Node;

/**
 * @struct ComponentDescriptor
 * @brief Intermediate data structure representing a parsed netlist line.
 * 
 * decouples the parsing concern (text to data) from the construction concern
 * (data to Component instance).
 */
struct ComponentDescriptor {
    bool is_directive = false;               // Starts with '.' (e.g., .ac, .dc)
    char type = '\0';                        // Uppercase first letter of ID (e.g., 'R', 'V')
    std::string id;                          // e.g., "R1", "V_source"
    std::string node1;                       // Positive terminal node name
    std::string node2;                       // Negative terminal node name
    std::vector<double> positional;          // List of bare number values
    std::map<std::string, double> keyed;     // Key-value pairs (e.g., "DC" -> 5.0, "AC" -> 1.0)
    std::vector<std::string> raw_tokens;     // Raw tokens (for directives or future commands)

    Node* ni = nullptr; // Resolved positive Node memories
    Node* nj = nullptr; // Resolved negative Node memories

};

#endif
