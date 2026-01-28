#include "solver.h"

Solver::Solver(const std::string& ac_output_file, int max_iter, double tolerance, double damping_factor)
    : gauss_seidel(max_iter, tolerance, damping_factor),
      gauss_seidel_ac(max_iter, tolerance, damping_factor),
      ac_analyzer(ac_output_file),
      duration(0), ac_duration(0) {}

// Dc solver
void Solver::solve_MNA_system(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                              const std::unordered_map<int, double>& mna_vector,
                              std::vector<double>& solution) {
    solution.resize(mna_matrix.size()+1, 0.0);
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    gauss_seidel.solve(mna_matrix, mna_vector, solution);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

// AC solver
void Solver::assemble_ac_system(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                                const std::map<int, std::string>& extra_vars,
                                const std::vector<double>& initial_solution) {
    ac_analyzer.initialize(mna_matrix, extra_vars, initial_solution);
}

void Solver::get_ac_response(const std::unordered_map<std::string, Component*>& ac_components,
                             double frequency) {
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    ac_analyzer.assemble_ac_mna_system(ac_components, frequency);
    gauss_seidel_ac.solve(ac_analyzer.mna_matrix, ac_analyzer.mna_vector, ac_analyzer.solution);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    ac_analyzer.log_ac_inst_solution(frequency, duration, gauss_seidel_ac.converge_iters);
}

void Solver::solve_ac_system(const std::unordered_map<std::string, Component*>& ac_components,
                             double freq1, double freq2, double step, bool log_scale) {
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    ac_analyzer.assemble_ac_mna_system(ac_components, 0.0); // Initial assembly at DC
    for (double freq = freq1; freq <= freq2; freq = log_scale ? freq * step : freq + step) 
        get_ac_response(ac_components, freq);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    ac_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int num_points = static_cast<int>((freq2 - freq1) / step) + 1;
    avg_ac_duration = static_cast<int>(ac_duration.count()) / num_points;
}

void Solver::print(std::ostream& os) const {
    if(gauss_seidel.converge_iters == 0) {
        os << "No solution available. Please run DC analysis first." << std::endl;
        return;
    }
    os << gauss_seidel;
    os << "  DC Solve Time Taken: " << duration.count() << " microseconds\n" << std::endl;

    if (ac_duration.count() <= 0)
        return;
    
    os << ac_analyzer;
    os << "  AC Solve Time Taken: " << ac_duration.count() << " microseconds" << std::endl;
    os << "  AC Average Time per Frequency Point: " << avg_ac_duration << " microseconds" << std::endl;
}