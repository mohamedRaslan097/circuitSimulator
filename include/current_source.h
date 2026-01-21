/**
 * @file current_source.h
 * @brief Independent current source component class for the circuit simulator.
 * 
 * Implements an ideal independent DC current source that supplies
 * a constant current regardless of the voltage across its terminals.
 */

#ifndef CURRENT_SOURCE_H
#define CURRENT_SOURCE_H

#include "component.h"

/**
 * @class Current_source
 * @brief Represents an ideal independent DC current source.
 * 
 * A current source supplies a specified current from its negative
 * terminal to its positive terminal (conventional current direction).
 * The voltage across the source is determined by the external circuit.
 * 
 * **MNA Stamp Pattern** (for current source I from node j to node i):
 * ```
 * Vector contributions only (no matrix stamps):
 *   b[i] -= I   (current leaves node i)
 *   b[j] += I   (current enters node j)
 * ```
 * 
 * Current flows from nj (negative) to ni (positive) through the source,
 * meaning ni is where current exits and nj is where it enters.
 * 
 * @note Current sources do not add extra variables to the MNA system,
 *       unlike voltage sources.
 * 
 * @see Component, Voltage_source, Component_contribution
 * 
 * @example
 * @code
 * Node* n1 = new Node("1");
 * Node* ground = new Node("0");
 * Current_source is("I1", n1, ground, 0.001);  // 1mA source
 * @endcode
 */
class Current_source : public Component {
public:
    static constexpr const char* default_id = "I";          // Default prefix for current source IDs
    static constexpr const char* type = "Current Source";   // Component type name for display
    
protected:
    double current;  // Source current in Amperes (A)
    
public:
    /**
     * @brief Constructs a current source with specified ID and current.
     * @param id Unique identifier (e.g., "I1", "Ibias").
     * @param ni Positive terminal node (current exits here).
     * @param nj Negative terminal node (current enters here).
     * @param c Source current in Amperes (default: 0).
     */
    Current_source(const std::string& id, Node* ni, Node* nj, double c = 0);
    
    /**
     * @brief Constructs a current source with auto-generated ID.
     * @param ni Positive terminal node.
     * @param nj Negative terminal node.
     * @param c Source current in Amperes.
     */
    Current_source(Node* ni, Node* nj, double c);
    
    /**
     * @brief Calculates voltage drop across the current source.
     * @return Voltage drop in Volts.
     * @throws std::runtime_error if node voltages are not valid.
     */
    virtual double get_voltage_drop() override;
    
    /**
     * @brief Returns the specified source current.
     * @return Source current in Amperes.
     */
    virtual double get_current() override;
    
    /**
     * @brief Generates MNA contributions (vector stamps only).
     * @return Component_contribution with current stamps to RHS vector.
     */
    virtual Component_contribution get_contribution() override;
    
    /**
     * @brief Prints current source information.
     * @param os Output stream (default: std::cout).
     * 
     * Format: "I(id)  node+ node-  value mA"
     */
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif