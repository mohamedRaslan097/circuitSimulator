/**
 * @file component_contribution.h
 * @brief Data structures for MNA matrix stamping contributions.
 * 
 * Components in the circuit contribute to the Modified Nodal Analysis (MNA)
 * system through "stamps" - predefined patterns of values added to the
 * system matrix and excitation vector. This file defines the structures
 * used to collect and apply these contributions.
 */

#ifndef COMPONENT_CONTRIBUTION_H
#define COMPONENT_CONTRIBUTION_H

#include <I_printable.h>
#include <vector>

/**
 * @struct MatrixContribution
 * @brief Represents a single contribution to the MNA system matrix.
 * 
 * Used to store a value that should be added to a specific position
 * (row, col) in the conductance/admittance matrix.
 */
struct MatrixContribution {
    int row;        // Row index in the MNA matrix (corresponds to equation)
    int col;        // Column index in the MNA matrix (corresponds to variable)
    double value;   // Value to add at position (row, col)
    
    /**
     * @brief Constructs a matrix contribution entry.
     * @param r Row index.
     * @param c Column index.
     * @param v Value to stamp.
     */
    MatrixContribution(int r, int c, double v);
};

/**
 * @struct VectorContribution
 * @brief Represents a single contribution to the MNA excitation vector.
 * 
 * Used to store a value that should be added to a specific position
 * in the right-hand side (RHS) vector of the linear system.
 */
struct VectorContribution {
    int row;        // Row index in the RHS vector
    double value;   // Value to add at the given row
    
    /**
     * @brief Constructs a vector contribution entry.
     * @param r Row index.
     * @param v Value to stamp.
     */
    VectorContribution(int r, double v);
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
 * @see Component::get_contribution()
 * 
 * @example
 * @code
 * // Resistor between nodes 1 and 2 with R = 1kΩ:
 * Component_contribution contrib;
 * double G = 1.0 / 1000.0;  // Conductance
 * contrib.stampMatrix(1, 1, G);   // +G at (1,1)
 * contrib.stampMatrix(2, 2, G);   // +G at (2,2)
 * contrib.stampMatrix(1, 2, -G);  // -G at (1,2)
 * contrib.stampMatrix(2, 1, -G);  // -G at (2,1)
 * @endcode
 */
class Component_contribution : public I_Printable {
public:
    std::vector<MatrixContribution> matrixStamps;   // Collection of matrix stamps
    std::vector<VectorContribution> vectorStamps;   // Collection of vector stamps
    
    /**
     * @brief Adds a contribution to the MNA system matrix.
     * @param row Row index (equation number).
     * @param col Column index (variable number).
     * @param value Value to add at (row, col).
     */
    void stampMatrix(int row, int col, double value);
    
    /**
     * @brief Adds a contribution to the MNA excitation vector.
     * @param row Row index.
     * @param value Value to add.
     */
    void stampVector(int row, double value);
    
    /**
     * @brief Prints all stamps in a formatted output.
     * @param os Output stream (default: std::cout).
     */
    virtual void print(std::ostream& os = std::cout) const override;
};


#endif