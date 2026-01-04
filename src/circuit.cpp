#include "circuit.h"
#include <set>
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
    for (auto& pair : components) {
        delete pair.second;
    }
}

void Circuit::add_node(std::string& nodeId) {
    if (nodes.find(nodeId) == nodes.end()) {
        Node* newNode = new Node(nodeId);
        nodes[nodeId] = newNode;
        nodeId_map[newNode->id] = newNode->name;
    }
}

void Circuit::add_resistor(std::string& resistorId, std::string& node1, std::string& node2, double resistance) {
    if (resistance < 0)
        throw std::runtime_error("Resistor with ID " + resistorId + " has negative resistance.");
    
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
        extraVarId_map[voltageSource->get_vc_id()] = Voltage_source::stamping_id + voltageSourceId;
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

    std::streampos original_pos = file.tellg();
    std::string first_line;
    std::getline(file, first_line);
    if(circuit_name == default_name && first_line[0] == '*') 
        circuit_name = first_line.substr(2);
    else
        file.seekg(original_pos);

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

void Circuit::print_nodes(std::ostream& os) const {
    os << "Circuit Nodes:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "Node(ID)"<< std::setw(16) <<"Voltage" << std::endl;
    os << std::string(40, '-') << std::endl;
    for (const auto& pair : nodes)
        os << *(pair.second) << std::endl;
    os << std::endl;
}

void Circuit::print_components(std::ostream& os) const {
    os << "Circuit Components:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "T(ID)" << std::setw(7) << "(+)" << std::setw(6) << "(-)" << std::right << std::setw(16) << "Value" << " Unit" << std::endl;
    os << std::string(40, '-') << std::endl;
    for (const auto& pair : components) 
        os << *(pair.second);
}

void Circuit::print(std::ostream& os) const {
    os << std::string(40, '=') << std::endl;
    os << "Circuit Name: " << circuit_name << std::endl;
    os << std::string(40, '=') << std::endl << std::endl;

    print_components(os);

    if(mna_matrix.size() > 0)
        print_MNA_system(os);

    if(Node::valid)
        print_solution(os);
    else
        print_nodes(os);
    
}

void Circuit::assemble_MNA_system() {
    mna_matrix.clear();
    for(const auto& component : components) {
        Component_contribution contrib = component.second->get_contribution();

        for(const auto& mc : contrib.matrixStamps)
            mna_matrix[mc.row][mc.col] += mc.value;
        
        for(const auto& vc : contrib.vectorStamps)
            mna_vector[vc.row] += vc.value;
    }
}

void Circuit::print_MNA_system(std::ostream& os) const {
    // Array of pointers to iterate both maps without merging
    const std::map<int, std::string>* var_maps[] = {&nodeId_map, &extraVarId_map};
    
    // Print MNA system in Matrix form
    os << "\nCircuit MNA System:" << std::endl;
    os << std::string(40, '-') << std::endl;

    // Print header row
    os << std::setw(6) << " ";
    for(const auto* map : var_maps) {
        for(const auto& var : *map) {
            os << std::setw(10) << var.second << " ";
        }
    }
    os << std::setw(5) << "|" << std::setw(10) << "RHS" << std::endl;
    
    // Print each row
    for(const auto* row_map : var_maps) {
        for(const auto& row : *row_map) {
            auto row_idx = row.first;
            auto row_it = mna_matrix.find(row_idx);

            os << std::setw(4) << row.second << "  [ ";

            // Print all columns
            for(const auto* col_map : var_maps) {
                for(const auto& col : *col_map) {
                    double value = 0.0;
                    if(row_it != mna_matrix.end()) {
                        auto col_it = row_it->second.find(col.first);
                        if(col_it != row_it->second.end()) {
                            value = col_it->second;
                        }
                    }
                    os << std::setw(10) << value << " ";
                }
            }

            double vec_value = 0.0;
            auto it_vec = mna_vector.find(row_idx);
            if(it_vec != mna_vector.end()) {
                vec_value = it_vec->second;
            }
            os << "]   [ " << std::setw(10) << vec_value << " ]" << std::endl;
        }
    }
    
    // Print extra variables info after the matrix
    if(extraVarId_map.size() > 0) {
        os << "\nExtra variables (" << extraVarId_map.size() << "): ";
        for(const auto& extra_var : extraVarId_map) {
            os << extra_var.second << " ";
        }
        os << std::endl;
    }
}

void Circuit::deploy_solution(const std::vector<double>& solution) {
    for (size_t i = 1; i < solution.size(); i++)
    {
        if (nodeId_map.find(i) != nodeId_map.end()) {
            std::string node_name = nodeId_map.at(i);
            nodes[node_name]->voltage = solution[i];
        } else if (extraVarId_map.find(i) != extraVarId_map.end()) {
            std::string componednt_name = extraVarId_map.at(i).substr(1);
            dynamic_cast<Voltage_source*>(components[componednt_name])->set_current(solution[i]);
        }else {
            throw std::runtime_error("Solution index " + std::to_string(i) + " does not correspond to any node or source.");
        }
    }
    Node::valid = true;
}

void Circuit::print_extraVars(std::ostream& os) const {
    os << "Circuit VS Currents:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "I_VS(ID)"<< std::setw(16) <<"Current" << std::endl;
    os << std::string(40, '-') << std::endl;
    for (const auto& [id, component_name] : extraVarId_map)
    os << std::left << std::setw(10) << "I_VS(" + component_name.substr(1) + ")"
       << std::right << std::fixed << std::setprecision(6) << std::setw(14) 
       << components.at(component_name.substr(1))->get_current() << " A"<< std::endl;
}

void Circuit::print_solution(std::ostream& os) const {
    os << "\n╔════════════════════════════════════╗\n"
       <<   "║   DC ANALYSIS RESULTS              ║\n"
       <<   "╚════════════════════════════════════╝\n\n";
    print_nodes(os);
    print_extraVars(os);
}