#include "gauss_seidel.h"
#include <cmath>
#include <iostream>
#include <iomanip>

// Set to true to enable debug output
static constexpr bool DEBUG_SOLVER = false;

Gauss_seidel::Gauss_seidel(int max_iter, double tolerance, double damping_factor)
    : max_iter(max_iter), tolerance(tolerance), damping_factor(damping_factor), 
      converge_iters(0), converged(false) {}

void Gauss_seidel::initialize(size_t size) {
    lhs_values.assign(size, 0.0);
    targets.resize(size);
    independent_targets.clear();
    for (size_t i = 0; i < size; i++)
        targets[i] = static_cast<int>(i);
    converge_iters = 0;
    converged = false;
}

void Gauss_seidel::handle_zero_diagonal(int row, const std::unordered_map<int, double>& col_map) {
    // Check if current target has zero or missing diagonal
    if (col_map.find(targets[row]) != col_map.end() && col_map.at(targets[row]) != 0.0)
        return;

    double max_val = 0.0;
    int max_idx = targets[row];
    bool independent = true;
    
    // Find index of the one with max non-used target value
    for (const auto& [col, value] : col_map) {
        // Skip zero values
        if(value == 0.0)
            continue;
        
        // skip independent variable cols - already used
        if (independent_targets.find(col) != independent_targets.end())
            continue;
        
        // If multiple non-independent variables found - not independent
        if(max_idx != targets[row])
            independent = false;
        
        // Value smaller than current max
        if (std::abs(value) <= max_val)
            continue;

        // New max found
        max_val = std::abs(value);
        max_idx = col;
    }
    
    // Row full of zeros
    if (max_idx == targets[row])
        return;
    
    // Update independent variables
    if (independent)
        independent_targets.insert(max_idx);
    
    // swap targets targets[x]=max_idx and targets[row]
    for (size_t i = 0; i < targets.size(); i++) {
        if (targets[i] == max_idx) {
            targets[i] = targets[row];
            break;
        }
    }
    targets[row] = max_idx;
}

double Gauss_seidel::compute_row_update(int row, const std::unordered_map<int, double>& col_map, const std::unordered_map<int, double>& mna_vector, std::vector<double>& solution) {
    double sum = 0.0;
    double diag = 0.0;
    double rhs_value = 0.0;

    for (const auto& [col, value] : col_map) {
        if (col != targets[row])
            sum += value * solution[col];
        else
            diag = value;
    }
    
    if (mna_vector.find(row) != mna_vector.end())
        rhs_value = mna_vector.at(row);
    
    double x_new = (rhs_value - sum) / diag;
    solution[targets[row]] = damping_factor * x_new + (1 - damping_factor) * solution[targets[row]];
    
    double lhs_value = sum + diag * solution[targets[row]];
    return lhs_value;
}

bool Gauss_seidel::check_convergence(const std::unordered_map<int, double>& mna_vector, size_t size) {
    for (size_t i = 0; i < size; i++) {
        auto rhs = mna_vector.find(static_cast<int>(i)) != mna_vector.end() 
                   ? mna_vector.at(static_cast<int>(i)) : 0.0;
        if (std::abs(lhs_values[i] - rhs) > tolerance) 
            return false;
    }
    return true;
}

void Gauss_seidel::dc_solve(const std::unordered_map<int, std::unordered_map<int, double>>& mna_matrix, const std::unordered_map<int, double>& mna_vector, std::vector<double>& solution) {
    
    initialize(solution.size());

    if (DEBUG_SOLVER) {
        std::cout << "\n========== GAUSS-SEIDEL DEBUG ==========\n";
        std::cout << "Solution size: " << solution.size() << "\n";
        std::cout << "MNA Matrix rows: " << mna_matrix.size() << "\n";
        std::cout << "\nMNA Matrix contents:\n";
        for (const auto& [row, col_map] : mna_matrix) {
            std::cout << "  Row " << row << ": ";
            for (const auto& [col, val] : col_map) {
                std::cout << "[" << col << "]=" << val << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\nMNA Vector (RHS):\n";
        for (const auto& [row, val] : mna_vector) {
            std::cout << "  [" << row << "] = " << val << "\n";
        }
        std::cout << "\nInitial targets: ";
        for (size_t i = 0; i < targets.size(); i++) {
            std::cout << targets[i] << " ";
        }
        std::cout << "\n";
    }
    
    for (converge_iters = 1; converge_iters < max_iter; converge_iters++) {
        if (DEBUG_SOLVER ) {
            std::cout << "\n--- Iteration " << converge_iters << " ---\n";
        }
        
        for (const auto& [row, col_map] : mna_matrix) {
            // Handle zero diagonal through dynamic pivoting
            int old_target = targets[row];
            handle_zero_diagonal(row, col_map);
            
            if (DEBUG_SOLVER  && old_target != targets[row]) {
                std::cout << "  Row " << row << ": target changed from " << old_target << " to " << targets[row] << "\n";
                std::cout << "  Targets now: ";
                for (size_t i = 0; i < targets.size(); i++) {
                    std::cout << targets[i] << " ";
                }
                std::cout << "\n";
            }
            
            // Skip rows that are all zeros
            if (col_map.find(targets[row]) == col_map.end() || col_map.at(targets[row]) == 0.0) {
                if (DEBUG_SOLVER ) {
                    std::cout << "  Row " << row << ": SKIPPED (zero diagonal at target " << targets[row] << ")\n";
                }
                continue;
            }
            
            // Compute and apply row update
            double old_val = solution[targets[row]];
            lhs_values[row] = compute_row_update(row, col_map, mna_vector, solution);
            
            if (DEBUG_SOLVER ) {
                std::cout << "  Row " << row << " solving for x[" << targets[row] << "]: " 
                          << std::fixed << std::setprecision(6) << old_val << " -> " << solution[targets[row]] 
                          << " (LHS=" << lhs_values[row] << ")\n";
            }
        }
        
        if (DEBUG_SOLVER ) {
            std::cout << "  Solution: ";
            for (size_t i = 0; i < solution.size(); i++) {
                std::cout << "x[" << i << "]=" << std::fixed << std::setprecision(6) << solution[i] << " ";
            }
            std::cout << "\n";
        }
        
        // Check convergence every 5 iterations
        if (converge_iters % 5 != 0)
            continue;
        
        converged = check_convergence(mna_vector, solution.size());
        if (DEBUG_SOLVER ) {
            std::cout << "  Convergence check: " << (converged ? "CONVERGED" : "not yet") << "\n";
        }
        if (converged)
            return;
    }
    
    if (DEBUG_SOLVER) {
        std::cout << "\n========== SOLVER FINISHED (max iter reached) ==========\n";
    }
}

void Gauss_seidel::print(std::ostream& os) const {
    os << "Modifed Gauss-Seidel Configuration:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "  Max Iterations: " << max_iter << std::endl;
    os << "  Tolerance: " << std::scientific << tolerance << std::endl;
    os << "  Damping Factor: " << std::fixed << damping_factor << std::endl;
    os << std::endl;
    os << "Gauss-Seidel Status:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "  Converged: " << (converged ? "Yes" : "No") << std::endl;
    os << "  Iterations Taken: " << converge_iters << std::endl;
}