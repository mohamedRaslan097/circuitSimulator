#include "circuit_printer.h"
#include "circuit.h"
#include <iomanip>

void CircuitPrinter::print_nodes(const Circuit& circuit, std::ostream& os) {
    os << "Circuit Nodes:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "Node(ID)"<< std::setw(16) <<"Voltage" << std::endl;
    os << std::string(40, '-') << std::endl;
    for (const auto& pair : circuit.nodeId_map)
        os << *(circuit.nodes.at(pair.second)) << std::endl;
    os << std::endl;
}

void CircuitPrinter::print_components(const Circuit& circuit, std::ostream& os) {
    os << "Circuit Components:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "T(ID)" << std::setw(7) << "(+)" << std::setw(6) << "(-)" << std::right << std::setw(16) << "Value" << " Unit" << std::endl;
    os << std::string(40, '-') << std::endl;
    for (const auto& pair : circuit.components) 
        os << *(pair.second);
}

void CircuitPrinter::print_MNA_system(const Circuit& circuit, std::ostream& os) {
    // Array of pointers to iterate both maps without merging
    const std::map<int, std::string>* var_maps[] = {&circuit.nodeId_map, &circuit.extraVarId_map};
    
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
            auto row_it = circuit.mna_matrix.find(row_idx);

            os << std::setw(4) << row.second << "  [ ";

            // Print all columns
            for(const auto* col_map : var_maps) {
                for(const auto& col : *col_map) {
                    double value = 0.0;
                    if(row_it != circuit.mna_matrix.end()) {
                        auto col_it = row_it->second.find(col.first);
                        if(col_it != row_it->second.end()) {
                            value = col_it->second;
                        }
                    }
                    os << std::setw(10) << value << " ";
                }
            }

            double vec_value = 0.0;
            auto it_vec = circuit.mna_vector.find(row_idx);
            if(it_vec != circuit.mna_vector.end()) {
                vec_value = it_vec->second;
            }
            os << "]   [ " << std::setw(10) << vec_value << " ]" << std::endl;
        }
    }
    
    // Print extra variables info after the matrix
    if(circuit.extraVarId_map.size() > 0) {
        os << "\nExtra variables (" << circuit.extraVarId_map.size() << "): ";
        for(const auto& extra_var : circuit.extraVarId_map) {
            os << extra_var.second << " ";
        }
        os << std::endl << std::endl;
    }
}

void CircuitPrinter::print_extraVars(const Circuit& circuit, std::ostream& os) {
    os << "Circuit VS Currents:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "I_VS(ID)"<< std::setw(16) <<"Current" << std::endl;
    os << std::string(40, '-') << std::endl;
    for (const auto& [id, component_name] : circuit.extraVarId_map)
    os << std::left << std::setw(10) << "I_VS(" + component_name.substr(1) + ")"
       << std::right << std::fixed << std::setprecision(6) << std::setw(14) 
       << circuit.components.at(component_name.substr(1))->get_current() * 1000.0 << " mA"<< std::endl;
}

void CircuitPrinter::print_solution(const Circuit& circuit, std::ostream& os) {
    os << "\n╔════════════════════════════════════╗\n"
       <<   "║   DC ANALYSIS RESULTS              ║\n"
       <<   "╚════════════════════════════════════╝\n\n";
    print_nodes(circuit, os);
    print_extraVars(circuit, os);
}

void CircuitPrinter::print(const Circuit& circuit, std::ostream& os) {
    os << std::string(40, '=') << std::endl;
    os << "Circuit Name: " << circuit.circuit_name << std::endl;
    os << std::string(40, '=') << std::endl << std::endl;

    print_components(circuit, os);

    if(circuit.mna_matrix.size() > 0)
        print_MNA_system(circuit, os);

    if(Node::valid)
        print_solution(circuit, os);
    else
        print_nodes(circuit, os);
}
