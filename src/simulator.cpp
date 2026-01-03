#include "simulator.h"

void Simulator::run_dc_analysis(const Circuit& circuit) {
    const auto& mna_matrix = circuit.get_MNA_matrix();
    const auto& mna_vector = circuit.get_MNA_vector();
    
    // Solve the MNA system
    solver.solve_MNA_system(mna_matrix, mna_vector, solution);
}