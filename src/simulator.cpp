#include "simulator.h"
#include "circuit.h"

void Simulator::run_dc_analysis(Circuit& circuit) {
    const auto& mna_matrix = circuit.get_MNA_matrix();
    const auto& mna_vector = circuit.get_MNA_vector();
    
    solver.solve_MNA_system(mna_matrix, mna_vector, solution);
    circuit.deploy_dc_solution(solution);
}

void Simulator::print(std::ostream& os) const {
    if(solution.empty()) {
        os << "No solution available. Please run DC analysis first." << std::endl;
        return;
    }
    os << solver << std::endl;
    os << "DC Raw Solution:" << std::endl;
    os << std::string(40, '-') << std::endl;
    for (size_t i = 0; i < solution.size(); i++) {
        os << "x[ " << i << std::setw(2) << " ]" << std::setw(2) << " = " << solution[i] << std::endl;
    }
}