#include "solver.h"
#include <set>

Solver::Solver(int max_iter, double tolerance, double damping_factor)
    : max_iter(max_iter), tolerance(tolerance), damping_factor(damping_factor), converge_iters(0), converged(false) {}

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
    std::vector<int> targets(solution.size(), 0);
    std::set<int> used_targets;
    for (size_t i =0; i < targets.size(); i++)
        targets[i] = i;
    
    for (converge_iters = 1; converge_iters < max_iter; converge_iters++)
    {
        for (auto& [row, col_map] : mna_matrix)
        {
            // Assign Unique Target in case of conflict
            if (col_map.find(targets[row]) == col_map.end() || col_map.at(targets[row]) == 0.0){
                double max_val = 0.0;
                int max_idx = row;
                // find index of the one with max non used target value
                for (auto& [col, value] : col_map){
                    // Value smaller than current max
                    if (std::abs(value) <= max_val || value == 0.0)
                        continue;
                    // col is already used
                    if (used_targets.find(col) != used_targets.end()){
                        // No target found yet
                        if (max_idx == row)
                            max_idx = col;
                        continue;
                    }
                    // New max found
                    max_val = std::abs(value);
                    max_idx = col;
                }
                // Row full of zeros
                if (max_idx == row)
                    continue;
                // Swap targets
                used_targets.insert(targets[row]);
                used_targets.insert(targets[targets[max_idx]]);
                std::swap(targets[row], targets[targets[max_idx]]);
                    // In case max_idx returned with already used target
                    // this means this is the only option left
                    // so we swap with current row
                    // and let the other row find new target next iterations
            }

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
            
            double x_new = (rhs_value - sum) / diag;
            solution[targets[row]] = damping_factor * x_new + (1 - damping_factor) * solution[targets[row]];
            lhs_value = sum + diag * solution[targets[row]];
            lhs_values[row] = lhs_value;
        }
        // Check convergence
        if (converge_iters % 5 != 0)
            continue;
        converged = true;
        for (size_t i = 0; i < solution.size(); i++) {
            auto rhs = mna_vector.find(i) != mna_vector.end() ? mna_vector.at(i) : 0.0;
            if (std::abs(lhs_values[i] - rhs) > tolerance) {
                converged = false;
                break;
            }
        }
        if (converged)
            return;
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