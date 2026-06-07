#ifndef CIRCUIT_BUILDER_H
#define CIRCUIT_BUILDER_H

#include "circuit.h"
#include <string>

/**
 * @class CircuitBuilder
 * @brief Class responsible for building and orchestrating the parsed netlist into a Circuit structure.
 */
class CircuitBuilder {
public:
    /**
     * @brief Parses and builds a complete Circuit structure from the netlist file.
     * @param circuit The Circuit instance to be populated.
     * @param filename Path to the netlist file.
     * @throws std::runtime_error if file cannot be opened.
     */
    void build(Circuit& circuit, const std::string& filename);
};

#endif // CIRCUIT_BUILDER_H
