#include "circuit.h"
#include <set>
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
    } else {
        throw std::runtime_error("Resistor with ID " + resistorId + " already exists in the circuit.");
    }
}

void Circuit::add_voltage_source(std::string& voltageSourceId, std::string& node1, std::string& node2, double voltage) {
    if(components.find(voltageSourceId) == components.end()) {
        Voltage_source* voltageSource = new Voltage_source(voltageSourceId, nodes[node1], nodes[node2], voltage);
        components[voltageSourceId] = voltageSource;
        extra_variables++;
    } else {
        throw std::runtime_error("Voltage source with ID " + voltageSourceId + " already exists in the circuit.");
    }
}

void Circuit::add_current_source(std::string& currentSourceId, std::string& node1, std::string& node2, double current) {
    if(components.find(currentSourceId) == components.end()) {
        Current_source* currentSource = new Current_source(currentSourceId, nodes[node1], nodes[node2], current);
        components[currentSourceId] = currentSource;
    } else {
        throw std::runtime_error("Current source with ID " + currentSourceId + " already exists in the circuit.");
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

    for (const auto& pair : nodes)
        os << *(pair.second) << std::endl;
    
    os << std::endl;
    os << "Circuit Components:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "T(ID)" << std::setw(7) << "(+)" << std::setw(6) << "(-)" << std::right << std::setw(16) << "Value" << " Unit" << std::endl;
    os << std::string(40, '-') << std::endl;

    for (const auto& pair : components) 
        os << *(pair.second);
    
    if(mna_matrix.size() > 0)
        display_MNA_system(os);
}

void Circuit::assemble_MNA_system() {
    mna_matrix.clear();
    for(const auto& component : components) {
        Component_contribution contrib = component.second->get_contribution();
        for(const auto& mc : contrib.matrixStamps) 
            mna_matrix[{mc.row, mc.col}] += mc.value;
        for(const auto& vc : contrib.vectorStamps) 
            mna_vector[vc.row] += vc.value;
    }
}

void Circuit::display_MNA_system(std::ostream& os) const {
    // Collect all variables
    std::set<std::string> all_vars;
    for(const auto& entry : mna_matrix) {
        if(entry.first.first != "0") all_vars.insert(entry.first.first);
        if(entry.first.second != "0") all_vars.insert(entry.first.second);
    }
    for(const auto& entry : mna_vector) {
        if(entry.first != "0") all_vars.insert(entry.first);
    }
    
    std::vector<std::string> vars(all_vars.begin(), all_vars.end());
    
    // Print MNA system in Matrix form
    os << "\nCircuit MNA System:" << std::endl;
    os << std::string(40, '-') << std::endl;

    // Print header row
    os << std::setw(6) << " ";
    for(const auto& col : vars) {
        os << std::setw(10) << col << " ";
    }
    os << std::setw(5) << "|" << std::setw(10) << "RHS" << std::endl;
    
    // Print each row
    for(const auto& row : vars) {
        os << std::setw(4) << row << "  [ ";
        for(const auto& col : vars) {
            double value = 0.0;
            auto it = mna_matrix.find({row, col});
            if(it != mna_matrix.end()) {
                value = it->second;
            }
            os << std::setw(10) << value << " ";
        }
        double vec_value = 0.0;
        auto it_vec = mna_vector.find(row);
        if(it_vec != mna_vector.end()) {
            vec_value = it_vec->second;
        }
        os << "]   [ " << std::setw(10) << vec_value << " ]" << std::endl;
    }
    
    // Print extra variables info after the matrix
    if(extra_variables > 0) {
        os << "\nExtra variables (" << extra_variables << "): ";
        // Print last extra_variables variables from the vars list
        for(size_t i = vars.size() - extra_variables; i < vars.size(); i++) {
            if(i > vars.size() - extra_variables) os << ", ";
            os << vars[i];
        }
        os << std::endl;
    }
}