/**
 * @file resistor.h
 * @brief Resistor component class for the circuit simulator.
 * 
 * Implements a linear resistor following Ohm's Law: V = I * R.
 * Resistors contribute conductance (G = 1/R) to the MNA system matrix.
 */

#ifndef ResISTOR_H
#define ResISTOR_H

#include "component.h"

/**
 * @class Resistor
 * @brief Represents a linear resistor in the circuit.
 * 
 * A resistor is a passive two-terminal component that implements
 * Ohm's Law. In the MNA formulation, resistors contribute their
 * conductance (G = 1/R) to the system matrix using the standard
 * conductance stamp pattern.
 * 
 * **MNA Stamp Pattern** (for resistor between nodes i and j):
 * ```
 * Matrix contributions:
 *   A[i][i] += G
 *   A[j][j] += G
 *   A[i][j] -= G
 *   A[j][i] -= G
 * ```
 * 
 * @note Resistance must be positive. Zero resistance will cause division by zero.
 * 
 * @see Component, Component_contribution
 * 
 * @example
 * @code
 * Node* n1 = new Node("1");
 * Node* n2 = new Node("2");
 * Resistor r("R1", n1, n2, 1000.0);  // 1kΩ resistor
 * @endcode
 */
class Resistor : public Component{
public:
    static constexpr const char* default_id = "R";      // Default prefix for resistor IDs
    static constexpr const char* type = "Resistor";     // Component type name for display
    
protected:
    double resistance;  // Resistance value in Ohms (Ω)
    
public:
    /**
     * @brief Constructs a resistor with specified ID and resistance.
     * @param id Unique identifier (e.g., "R1", "R2").
     * @param ni Positive terminal node.
     * @param nj Negative terminal node.
     * @param r Resistance in Ohms (default: 0, must be positive in practice).
     */
    Resistor( const std::string& id, Node* ni, Node* nj, double r = 0);
    
    /**
     * @brief Constructs a resistor with auto-generated ID.
     * @param ni Positive terminal node.
     * @param nj Negative terminal node.
     * @param r Resistance in Ohms.
     */
    Resistor(Node* ni, Node* nj, double r);
    
    /**
     * @brief Calculates voltage drop across the resistor.
     * @return Voltage drop in Volts.
     * @throws std::runtime_error if node voltages are not valid.
     */
    virtual double get_voltage_drop() override;
    
    /**
     * @brief Calculates current through the resistor using Ohm's Law.
     * @return Current I = V/R in Amperes (positive from ni to nj).
     * @throws std::runtime_error if node voltages are not valid.
     */
    virtual double get_current() override;
    
    /**
     * @brief Generates conductance stamps for the MNA matrix.
     * @return Component_contribution with conductance stamp pattern.
     */
    virtual Component_contribution get_contribution() override;
    
    /**
     * @brief Prints resistor information.
     * @param os Output stream (default: std::cout).
     * 
     * Format: "R(id)  node+ node-  value kΩ"
     */
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif