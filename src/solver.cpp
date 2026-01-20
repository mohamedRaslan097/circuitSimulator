#include "solver.h"

Solver::Solver(int max_iter, double tolerance, double damping_factor)
    : gauss_seidel(max_iter, tolerance, damping_factor), duration(0) {}

void Solver::solve_MNA_system(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                              const std::unordered_map<int, double>& mna_vector,
                              std::vector<double>& solution) {
    solution.resize(mna_matrix.size()+1, 0.0);
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    gauss_seidel.dc_solve(mna_matrix, mna_vector, solution);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

void Solver::print(std::ostream& os) const {
    if(gauss_seidel.converge_iters == 0) {
        os << "No solution available. Please run DC analysis first." << std::endl;
        return;
    }
    os << "Solver Configuration:" << std::endl;
    os << gauss_seidel;
    os << "  Time Taken: " << duration.count() << " microseconds" << std::endl;

}