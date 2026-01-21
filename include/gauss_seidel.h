/**
 * @file gauss_seidel.h
 * @brief Gauss-Seidel iterative solver for linear systems.
 * 
 * Implements the Gauss-Seidel iterative method with enhancements for
 * handling zero diagonal elements (common in MNA systems with voltage
 * sources and inductors).
 */

#ifndef GAUSS_SEIDEL_H
#define GAUSS_SEIDEL_H

#include <unordered_map>
#include <vector>
#include <set>
#include "I_Printable.h"

/**
 * @class Gauss_seidel
 * @brief Gauss-Seidel iterative solver for sparse linear systems.
 * 
 * Implements the Gauss-Seidel method with modifications to handle
 * zero diagonal elements through dynamic pivoting. This is essential
 * for MNA systems which often have zero diagonals due to voltage
 * source and inductor stamps. This is done efficently on sparse matrix
 *  through independent target tracking.
 * 
 * 
 * **Algorithm:**
 * For system Ax = b, iterate until convergence:
 * ```
 * x_i^(k+1) = (1/A_ii) * (b_i - Σ(A_ij * x_j^(k+1), j<i) - Σ(A_ij * x_j^(k), j>i))
 * ```
 * 
 * With damping (under-relaxation):
 * ```
 * x_i^(k+1) = (1-ω)*x_i^(k) + ω*x_i^(new)
 * ```
 * 
 * **Zero Diagonal Handling:**
 * When A_ii = 0, the solver identifies rows that can be used to solve
 * for the variable by finding non-zero off-diagonal elements and
 * redirecting the computation.
 * 
 * **Convergence:**
 * The solver uses relative residual norm: ||Ax - b|| / ||b|| < tolerance
 * 
 * @note The Gauss-Seidel method works best for diagonally dominant systems.
 *       The damping factor helps stability for ill-conditioned systems.
 *       The Modifed Gauss-Seidel here is tailored for MNA systems in circuit analysis.
 *       It overcomes zero-diagonal issues by dynamically selecting alternative
 *       variables to solve for, ensuring progress in each iteration.
 *       Also tracks independent targets to avoid cyclic dependencies.
 * 
 * @see Solver
 */
class Gauss_seidel : public I_Printable {
    friend class Solver;  // Solver class has access to internal state
    
private:
    int max_iter;           // Maximum number of iterations allowed
    double tolerance;       // Convergence tolerance (relative residual)
    double damping_factor;  // Under-relaxation factor ω ∈ (0, 1]
    int converge_iters;     // Actual iterations taken to converge
    bool converged;         // Flag indicating successful convergence

    // Internal state for zero-diagonal handling
    std::vector<double> lhs_values;             // Stored LHS coefficients for zero-diagonal rows
    std::vector<int> targets;                   // Target variable indices for each row
    std::vector<int> var_to_target;             // Map variable index to row index
    std::set<int> independent_targets;          // Variables that need special handling

    /**
     * @brief Initializes internal data structures.
     * @param size Dimension of the linear system.
     */
    void initialize(size_t size);
    
    /**
     * @brief Handles rows with zero diagonal elements.
     * @param row Current row index.
     * @param col_map Map of column indices to values in this row.
     * 
     * Identifies alternative variables to solve for when the diagonal
     * element is zero.
     */
    void handle_zero_diagonal(int row, 
                              const std::unordered_map<int, double>& col_map);
    
    /**
     * @brief Computes the updated value for a single row.
     * @param row Row index.
     * @param col_map Map of column indices to values in this row.
     * @param mna_vector Right-hand side vector.
     * @param solution Current solution vector (updated in place).
     * @return The computed residual for this row.
     */
    double compute_row_update(int row,
                              const std::unordered_map<int, double>& col_map,
                              const std::unordered_map<int, double>& mna_vector,
                              std::vector<double>& solution);
    
    /**
     * @brief Checks if the solution has converged.
     * @param mna_vector Right-hand side vector.
     * @param size System dimension.
     * @return true if relative residual is below tolerance.
     */
    bool check_convergence(const std::unordered_map<int, double>& mna_vector,
                           size_t size);

public:
    /**
     * @brief Constructs a Gauss-Seidel solver with specified parameters.
     * @param max_iter Maximum iterations (default: 1000).
     * @param tolerance Convergence tolerance (default: 1e-9).
     * @param damping_factor Under-relaxation factor (default: 0.1).
     */
    Gauss_seidel(int max_iter = 1000, double tolerance = 1e-9, double damping_factor = 0.1);
    
    /**
     * @brief Solves the DC linear system using Gauss-Seidel iteration.
     * @param mna_matrix Sparse system matrix A.
     * @param mna_vector Right-hand side vector b.
     * @param solution Solution vector x (output, must be pre-sized).
     * 
     * @note The solution vector should be initialized (e.g., to zeros)
     *       before calling this method.
     * 
     * @par Time Complexity
     * O(I × N × K) where:
     * - I = number of iterations until convergence (≤ max_iter)
     * - N = system dimension (nodes + extra variables)
     * - K = average non-zeros per row (typically ≤ 5 for circuits)
     * 
     * For sparse MNA matrices: O(I × NNZ) where NNZ = total non-zeros
     * 
     * @par Space Complexity
     * O(N) for internal vectors (lhs_values, targets)
     */
    void dc_solve(const std::unordered_map<int, std::unordered_map<int, double>>& mna_matrix,
               const std::unordered_map<int, double>& mna_vector,
               std::vector<double>& solution);
    
    /**
     * @brief Prints solver configuration and convergence information.
     * @param os Output stream (default: std::cout).
     */
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif
