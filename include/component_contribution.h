/**
 * @file component_contribution.h
 * @brief Data structures for MNA matrix stamping contributions.
 * 
 * Components in the circuit contribute to the Modified Nodal Analysis (MNA)
 * system through "stamps" - predefined patterns of values added to the
 * system matrix and excitation vector. This file defines the structures
 * used to collect and apply these contributions.
 * 
 * @tparam T Numeric type for values (double or std::complex<double>)
 */

#ifndef COMPONENT_CONTRIBUTION_H
#define COMPONENT_CONTRIBUTION_H

#include <I_printable.h>
#include <vector>
#include <complex>

/**
 * @struct MatrixContribution
 * @brief Represents a single contribution to the MNA system matrix.
 * 
 * Used to store a value that should be added to a specific position
 * (row, col) in the conductance/admittance matrix.
 * 
 * @tparam T Numeric type (default: double)
 */
template<typename T = double>
struct MatrixContribution {
    int row;        // Row index in the MNA matrix (corresponds to equation)
    int col;        // Column index in the MNA matrix (corresponds to variable)
    T value;        // Value to add at position (row, col)
    
    /**
     * @brief Constructs a matrix contribution entry.
     * @param r Row index.
     * @param c Column index.
     * @param v Value to stamp.
     */
    MatrixContribution(int r, int c, T v);
};

/**
 * @struct VectorContribution
 * @brief Represents a single contribution to the MNA excitation vector.
 * 
 * Used to store a value that should be added to a specific position
 * in the right-hand side (RHS) vector of the linear system.
 * 
 * @tparam T Numeric type (default: double)
 */
template<typename T = double>
struct VectorContribution {
    int row;        // Row index in the RHS vector
    T value;        // Value to add at the given row
    
    /**
     * @brief Constructs a vector contribution entry.
     * @param r Row index.
     * @param v Value to stamp.
     */
    VectorContribution(int r, T v);
};

/**
 * @class Component_contribution
 * @brief Collects all MNA contributions from a single component.
 * 
 * Each circuit component generates stamps according to its type:
 * - **Resistors**: Stamp conductance (G = 1/R) to the matrix
 * - **Voltage sources**: Stamp ±1 values and voltage to vector
 * - **Current sources**: Stamp current directly to vector
 * - **Inductors**: Treated as short circuits in DC analysis
 * - **Capacitors**: Treated as open circuits in DC analysis
 * 
 * @tparam T Numeric type (default: double, can be std::complex<double> for AC)
 * 
 * @see Component::get_contribution()
 * 
 * @example
 * @code
 * // Resistor between nodes 1 and 2 with R = 1kΩ:
 * Component_contribution<> contrib;  // defaults to double
 * double G = 1.0 / 1000.0;  // Conductance
 * contrib.stampMatrix(1, 1, G);   // +G at (1,1)
 * contrib.stampMatrix(2, 2, G);   // +G at (2,2)
 * contrib.stampMatrix(1, 2, -G);  // -G at (1,2)
 * contrib.stampMatrix(2, 1, -G);  // -G at (2,1)
 * @endcode
 */
template<typename T = double>
class Component_contribution : public I_Printable {
public:
    std::vector<MatrixContribution<T>> matrixStamps;   // Collection of matrix stamps
    std::vector<VectorContribution<T>> vectorStamps;   // Collection of vector stamps
    
    /**
     * @brief Adds a contribution to the MNA system matrix.
     * @param row Row index (equation number).
     * @param col Column index (variable number).
     * @param value Value to add at (row, col).
     */
    void stampMatrix(int row, int col, T value);
    
    /**
     * @brief Adds a contribution to the MNA excitation vector.
     * @param row Row index.
     * @param value Value to add.
     */
    void stampVector(int row, T value);
    
    /**
     * @brief Prints all stamps in a formatted output.
     * @param os Output stream (default: std::cout).
     */
    virtual void print(std::ostream& os = std::cout) const override;
};

// Explicit template instantiation declarations
extern template struct MatrixContribution<double>;
extern template struct MatrixContribution<std::complex<double>>;
extern template struct VectorContribution<double>;
extern template struct VectorContribution<std::complex<double>>;
extern template class Component_contribution<double>;
extern template class Component_contribution<std::complex<double>>;

#endif