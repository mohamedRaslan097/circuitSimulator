#include "circuit.h"
#include "component_descriptor.h"
#include "netlist_parser.h"
#include "componentFactory.h"
#include "circuit_printer.h"

Circuit::Circuit(std::string name) : circuit_name(name) {
    nodes.clear();
    components.clear();
    Node* ground = new Node("0");
    nodes[ground->name] = ground;
}

Circuit::~Circuit() {
    for (auto& pair : nodes) {
        delete pair.second;
    }
    for (auto& pair : ac_components) {
        pair.second = nullptr; // Prevent double deletion
    }
    for (auto& pair : components) {
        delete pair.second;
    }
}

// Helper functions

void Circuit::add_node(std::string& nodeId) {
    if (nodes.find(nodeId) == nodes.end()) {
        Node* newNode = new Node(nodeId);
        nodes[nodeId] = newNode;
        nodeId_map[newNode->id] = newNode->name;
    }
}

void Circuit::add_component(const ComponentDescriptor& descriptor) {
    if(descriptor.is_directive) {
        throw std::runtime_error("Directives cannot be added as components to the circuit.");
    }

    if(components.find(descriptor.id) != components.end()) {
        throw std::runtime_error("Component with ID " + descriptor.id + " already exists in the circuit.");
    }

    ComponentFactory componentFactory;
    Component* component = componentFactory.create_component(descriptor);
    components[descriptor.id] = component;

    if(component->is_ac()) {
        ac_components[descriptor.id] = component;
        if(component->has_extra_var()) {
            extraVarId_map[component->get_vc_id()] = component->get_stamping_label();
        }
    }
}

// Core functions

void Circuit::assemble_MNA_system() {
    mna_matrix.clear();
    for(const auto& component : components) {
        Component_contribution<double> contrib = component.second->get_contribution();

        for(const auto& mc : contrib.matrixStamps)
            mna_matrix[mc.row][mc.col] += mc.value;
        
        for(const auto& vc : contrib.vectorStamps)
            mna_vector[vc.row] += vc.value;
    }
}

void Circuit::deploy_dc_solution(const std::vector<double>& solution) {
    for (size_t i = 1; i < solution.size(); i++)
    {
        if (nodeId_map.find(i) != nodeId_map.end()) {
            std::string node_name = nodeId_map.at(i);
            nodes[node_name]->voltage = solution[i];
        } else if (extraVarId_map.find(i) != extraVarId_map.end()) {
            std::string component_name = extraVarId_map.at(i).substr(1);
            components[component_name]->set_current(solution[i]);
        }else {
            throw std::runtime_error("Solution index " + std::to_string(i) + " does not correspond to any node or source.");
        }
    }
    Node::valid = true;
}

// Print functions

void Circuit::print_nodes(std::ostream& os) const {
    CircuitPrinter::print_nodes(*this, os);
}

void Circuit::print_components(std::ostream& os) const {
    CircuitPrinter::print_components(*this, os);
}

void Circuit::print_MNA_system(std::ostream& os) const {
    CircuitPrinter::print_MNA_system(*this, os);
}

void Circuit::print_extraVars(std::ostream& os) const {
    CircuitPrinter::print_extraVars(*this, os);
}

void Circuit::print_solution(std::ostream& os) const {
    CircuitPrinter::print_solution(*this, os);
}

void Circuit::print(std::ostream& os) const {
    CircuitPrinter::print(*this, os);
}