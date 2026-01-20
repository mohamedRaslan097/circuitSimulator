#ifndef GAUSS_SEIDEL_H
#define GAUSS_SEIDEL_H

#include <unordered_map>
#include <vector>
#include <set>
#include "I_Printable.h"

/**
 * @brief Gauss-Seidel iterative solver class
 * 
 * Implements the Gauss-Seidel method with modifications to handle
 * zero diagonal elements through dynamic pivoting.
 */
class Gauss_seidel : public I_Printable {
    friend class Solver;
private:
    int max_iter;
    double tolerance;
    double damping_factor;
    int converge_iters;
    bool converged;

    // Internal state for zero-diagonal handling
    std::vector<double> lhs_values;
    std::vector<int> targets;
    std::set<int> independent_targets;

    void initialize(size_t size);
    void handle_zero_diagonal(int row, 
                              const std::unordered_map<int, double>& col_map);
    double compute_row_update(int row,
                              const std::unordered_map<int, double>& col_map,
                              const std::unordered_map<int, double>& mna_vector,
                              std::vector<double>& solution);
    bool check_convergence(const std::unordered_map<int, double>& mna_vector,
                           size_t size);

public:
    Gauss_seidel(int max_iter = 1000, double tolerance = 1e-9, double damping_factor = 0.1);
    
    void dc_solve(const std::unordered_map<int, std::unordered_map<int, double>>& mna_matrix,
               const std::unordered_map<int, double>& mna_vector,
               std::vector<double>& solution);
    
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif
