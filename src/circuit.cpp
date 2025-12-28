#include "circuit.h"

Circuit::Circuit(std::string name) : circuit_name(name) {
    nodes.clear();
    components.clear();
    Node* ground = new Node("0");
    nodes[ground->id] = ground;
}

Circuit::~Circuit() {
    for (auto& pair : nodes) {
        delete pair.second;
    }
    for (auto& pair : components) {
        delete pair.second;
    }
}

void Circuit::add_node(std::string& nodeId) {
    if (nodes.find(nodeId) == nodes.end()) {
        Node* newNode = new Node(nodeId);
        nodes[nodeId] = newNode;
    }
}

void Circuit::add_resistor(std::string& resistorId, std::string& node1, std::string& node2, double resistance) {
    if(components.find(resistorId) == components.end()) {
        Resistor* resistor = new Resistor(resistorId, nodes[node1], nodes[node2], resistance);
        components[resistorId] = resistor;
    }
}

void Circuit::add_voltage_source(std::string& voltageSourceId, std::string& node1, std::string& node2, double voltage) {
    if(components.find(voltageSourceId) == components.end()) {
        Voltage_source* voltageSource = new Voltage_source(voltageSourceId, nodes[node1], nodes[node2], voltage);
        components[voltageSourceId] = voltageSource;
    }
}

void Circuit::add_current_source(std::string& currentSourceId, std::string& node1, std::string& node2, double current) {
    if(components.find(currentSourceId) == components.end()) {
        Current_source* currentSource = new Current_source(currentSourceId, nodes[node1], nodes[node2], current);
        components[currentSourceId] = currentSource;
    }
}

void Circuit::parse_netlist(const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open netlist file: " + filename);
    }
    
    std::string title;
    getline(file,title);
    
    circuit_name = title[0] == '*' ? title.substr(2) : title;

    std::string component_id;
    std::string node1_id;
    std::string node2_id;
    double value;
    
    while (file >> component_id)
    {
        if(component_id[0] == '*')
        {
            std::string comment_line;
            getline(file, comment_line);
            continue;
        }
        
        if(!(file >> node1_id >> node2_id >> value))
            throw std::runtime_error("Error parsing netlist line for component: " + component_id);
        
        add_node(node1_id);
        add_node(node2_id);
        
        if(toupper(component_id[0]) == 'R')
            add_resistor(component_id, node1_id, node2_id, value);
        else if(toupper(component_id[0]) == 'V')
            add_voltage_source(component_id, node1_id, node2_id, value);
        else if(toupper(component_id[0]) == 'I')
            add_current_source(component_id, node1_id, node2_id, value);
        else
            throw std::runtime_error("Unknown component type in netlist: " + component_id);
    }
}

void Circuit::print(std::ostream& os) const {
    os << std::string(40, '=') << std::endl;
    os << "Circuit Name: " << circuit_name << std::endl;
    os << std::string(40, '=') << std::endl;

    os << "\nCircuit Nodes:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "Node(ID)"<< std::setw(16) <<"Voltage" << std::endl;
    os << std::string(40, '-') << std::endl;
    for (const auto& pair : nodes) {
        os << *(pair.second) << std::endl;
    }
    os << std::endl;
    os << "Circuit Components:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "T(ID)" << std::setw(7) << "(-)" << std::setw(6) << "(+)" << std::right << std::setw(16) << "Value" << " Unit" << std::endl;
    os << std::string(40, '-') << std::endl;
    for (const auto& pair : components) {
        os << *(pair.second);
    }
}