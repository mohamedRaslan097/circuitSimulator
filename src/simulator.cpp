#include "simulator.h"
#include "circuit.h"

Simulator::Simulator(const std::string& ac_output_file) : solver(ac_output_file){}

void Simulator::run_dc_analysis(Circuit& circuit) {
    const auto& mna_matrix = circuit.get_MNA_matrix();
    const auto& mna_vector = circuit.get_MNA_vector();
    
    solver.solve_MNA_system(mna_matrix, mna_vector, solution);
    circuit.deploy_dc_solution(solution);
}

void Simulator::run_ac_analysis(Circuit& circuit, double freq1, double freq2, double step, bool log_scale) {
    if (freq1 <= 0)
        throw std::invalid_argument("Invalid start frequency: freq1 must be positive.");

    if (freq2 < freq1)
        throw std::invalid_argument("Invalid end frequency: freq2 must be greater than or equal to freq1.");

    if(step <= 0)
        throw std::invalid_argument("Invalid frequency step: step must be positive.");
    
    const auto& mna_matrix = circuit.get_MNA_matrix();
    const auto& extra_vars = circuit.get_extraVarId_map();
    const auto& ac_components = circuit.get_ac_components();

    solver.assemble_ac_system(mna_matrix, extra_vars, solution);
    solver.solve_ac_system(ac_components, freq1, freq2, step, log_scale);
}

void Simulator::run_ac_analysis(Circuit& circuit, double frequency) {
    run_ac_analysis(circuit, frequency, frequency, 1.0);
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