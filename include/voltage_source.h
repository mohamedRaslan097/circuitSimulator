/**
 * @file voltage_source.h
 * @brief Independent voltage source component class for the circuit simulator.
 * 
 * Implements an ideal independent DC voltage source that maintains
 * a constant voltage between its terminals regardless of current.
 */

#ifndef VOLTAGE_SOURCE_H
#define VOLTAGE_SOURCE_H

#include "Component.h"

/**
 * @class Voltage_source
 * @brief Represents an ideal independent DC voltage source.
 * 
 * A voltage source maintains a specified voltage between its positive
 * and negative terminals. The current through the source is determined
 * by the external circuit. In MNA, voltage sources require an extra
 * variable (the source current) to be added to the system.
 * 
 * **MNA Stamp Pattern** (for voltage source V between nodes i and j):
 * ```
 * Matrix contributions:
 *   A[i][vc_id] += 1      (KCL at node i includes I_source)
 *   A[j][vc_id] -= 1      (KCL at node j includes -I_source)
 *   A[vc_id][i] += 1      (V_i term in voltage equation)
 *   A[vc_id][j] -= 1      (V_j term in voltage equation)
 * Vector contributions:
 *   b[vc_id] = V          (V_i - V_j = V)
 * ```
 * 
 * The positive terminal (ni) is at higher potential than negative (nj).
 * Voltage drop = voltage_value
 * 
 * @note Current flows from positive to negative terminal inside the source
 *       (conventional current direction for a source supplying power).
 * 
 * @see Component, Component_contribution
 * 
 * @example
 * @code
 * Node* n1 = new Node("1");
 * Node* ground = new Node("0");
 * Voltage_source vs("V1", n1, ground, 5.0);  // 5V source
 * @endcode
 */
class Voltage_source : public Component {
public:
    static constexpr const char* default_id = "V";          // Default prefix for voltage source IDs
    static constexpr const char* stamping_id = "I";         // Prefix for current variable in extraVarId_map
    static constexpr const char* type = "Voltage Source";   // Component type name for display
    
protected:
    int vc_id;      // Index for the source current variable in MNA system
    double voltage; // Source voltage in Volts (V)
    double current; // Computed current through the source in Amperes
    
public:
    /**
     * @brief Constructs a voltage source with specified ID and voltage.
     * @param id Unique identifier (e.g., "V1", "Vcc").
     * @param ni Positive terminal node (higher potential).
     * @param nj Negative terminal node (lower potential).
     * @param v Source voltage in Volts (default: 0).
     * 
     * @note Automatically allocates an extra variable ID (vc_id) for the source current.
     */
    Voltage_source(const std::string& id, Node* ni, Node* nj, double v = 0);
    
    /**
     * @brief Constructs a voltage source with auto-generated ID.
     * @param ni Positive terminal node.
     * @param nj Negative terminal node.
     * @param v Source voltage in Volts.
     */
    Voltage_source(Node* ni, Node* nj, double v);
    
    /**
     * @brief Returns the voltage source value.
     * @return Source voltage in Volts.
     * 
     * @note Unlike passive components, this returns the specified source
     *       voltage, not the computed voltage difference.
     */
    virtual double get_voltage_drop() override;
    
    /**
     * @brief Returns the computed current through the source.
     * @return Current in Amperes (set by deploy_dc_solution).
     */
    virtual double get_current() override;
    
    /**
     * @brief Generates MNA contributions for the voltage source.
     * @return Component_contribution with standard voltage source stamps.
     */
    virtual Component_contribution get_contribution() override;
    
    /**
     * @brief Prints voltage source information.
     * @param os Output stream (default: std::cout).
     * 
     * Format: "V(id)  node+ node-  value V"
     */
    virtual void print(std::ostream& os = std::cout) const override;
    
    /**
     * @brief Gets the extra variable index for source current.
     * @return Index in the MNA system solution vector.
     */
    int get_vc_id() const { return vc_id; }
    
    /**
     * @brief Sets the source current (called after solving MNA system).
     * @param i Current value in Amperes.
     */
    void set_current(double i) { current = i; }
};

#endif