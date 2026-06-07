#include "circuit_builder.h"
#include "netlist_parser.h"
#include <fstream>
#include <stdexcept>

void CircuitBuilder::build(Circuit& circuit, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open netlist file: " + filename);
    }

    std::string header_name = NetlistParser::parse_header(file);
    if (!header_name.empty() && circuit.circuit_name == Circuit::default_name) {
        circuit.circuit_name = header_name;
    }

    std::string line;
    while (std::getline(file, line)) {
        ComponentDescriptor descriptor;
        if (!NetlistParser::parse_line(line, descriptor)) {
            continue;
        }

        // Add nodes to the circuit
        circuit.add_node(descriptor.node1);
        circuit.add_node(descriptor.node2);

        // Resolve node pointers inside descriptor before creation
        descriptor.ni = circuit.nodes[descriptor.node1];
        descriptor.nj = circuit.nodes[descriptor.node2];

        // Add component to the circuit
        circuit.add_component(descriptor);
    }
}
