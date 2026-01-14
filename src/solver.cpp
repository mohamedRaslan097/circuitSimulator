#include "solver.h"
Solver::Solver(int max_iter, double tolerance)
    : max_iter(max_iter), tolerance(tolerance), converge_iters(0), converged(false) {}

void Solver::solve_MNA_system(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                              const std::unordered_map<int, double>& mna_vector,
                              std::vector<double>& solution) {
    solution.resize(mna_matrix.size()+1, 0.0);
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    gauss_seidel_solver(mna_matrix, mna_vector, solution);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

void Solver::gauss_seidel_solver(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                                 const std::unordered_map<int, double>& mna_vector,
                                 std::vector<double>& solution) {
    
    // Hack to Overcome zero diagonal issues
    std::vector<double> lhs_values(solution.size(), 0.0);
    std::vector<double> targets(solution.size(), 0.0);
    for (size_t i =0; i < targets.size(); i++)
        targets[i] = i;
    
    for (converge_iters = 1; converge_iters < max_iter; converge_iters++)
    {
        for (auto& [row, col_map] : mna_matrix)
        {
            if (col_map.find(targets[row]) == col_map.end() || col_map.at(targets[row]) == 0.0){
                // find index of the one with max value
                double max_val = INT_MIN;
                int max_idx = -1;
                for (auto& [col, value] : col_map)
                    if (value > max_val){
                        max_val = value;
                        max_idx = col;
                    }
                std::swap(targets[row], targets[max_idx]);
            }
            //     throw std::runtime_error("Zero diagonal element detected in MNA matrix at row " + std::to_string(row));
            
            double sum = 0.0;
            double diag = 0.0;
            double rhs_value = 0.0;
            double lhs_value = 0.0;

            for (const auto& [col, value] : col_map)
                if (col != targets[row])
                    sum += value * solution[col];
                else
                    diag = value;
            
            if (mna_vector.find(row) != mna_vector.end())
                rhs_value = mna_vector.at(row);
            
            solution[targets[row]] = (rhs_value - sum) / diag;
            lhs_value = sum + diag * solution[targets[row]];
            lhs_values[row] = lhs_value;
        }
        // Check convergence
        if (converge_iters % 5 != 0)
            continue;
        for (size_t i = 0; i < solution.size(); i++) {
            auto rhs = mna_vector.find(i) != mna_vector.end() ? mna_vector.at(i) : 0.0;
            if (std::abs(lhs_values[i] - rhs) < tolerance) {
                converged = true;
                return;
            }
        }
    }
}

void Solver::print(std::ostream& os) const {
    if(converge_iters == 0) {
        os << "No solution available. Please run DC analysis first." << std::endl;
        return;
    }
    os << "Solver Configuration:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "Method: " << "Modified Gauss-Seidel" << std::endl;
    os << "  Max Iterations: " << max_iter << std::endl;
    os << "  Tolerance: " << std::scientific << tolerance << std::endl;
    os << std::endl;
    os << "Solver Status:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "  Converged: " << (converged ? "Yes" : "No") << std::endl;
    os << "  Iterations Taken: " << converge_iters << std::endl;
    os << "  Time Taken: " << duration.count() << " microseconds" << std::endl;
}