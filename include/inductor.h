/**
 * @file inductor.h
 * @brief Inductor component class for the circuit simulator.
 * 
 * Implements a linear inductor with inductance L.
 * In DC analysis, inductors act as short circuits (zero voltage drop).
 */

#ifndef INDUCTOR_H
#define INDUCTOR_H

#include "Component.h"

/**
 * @class Inductor
 * @brief Represents a linear inductor in the circuit.
 * 
 * An inductor is a passive two-terminal component that stores energy
 * in a magnetic field. The voltage-current relationship is:
 * V = L * dI/dt
 * 
 * **DC Analysis Behavior:**
 * In steady-state DC analysis, dI/dt = 0, so inductors behave as
 * short circuits with zero voltage drop. The inductor is modeled
 * similarly to a voltage source with V = 0, requiring an extra
 * variable (current) in the MNA system.
 * 
 * **MNA Stamp Pattern** (for inductor between nodes i and j):
 * ```
 * Matrix contributions (like voltage source with V=0):
 *   A[i][vc_id] += 1
 *   A[vc_id][i] += 1
 *   A[j][vc_id] -= 1
 *   A[vc_id][j] -= 1
 * Vector contributions:
 *   b[vc_id] = 0  (zero voltage in DC)
 * ```
 * 
 * @note The inductor current is stored as an extra variable in the MNA system.
 * 
 * @see Component, Voltage_source, Component_contribution
 * 
 * @example
 * @code
 * Node* n1 = new Node("1");
 * Node* n2 = new Node("2");
 * Inductor l("L1", n1, n2, 1e-3);  // 1mH inductor
 * @endcode
 */
class Inductor : public Component {
public:
    static constexpr const char* default_id = "L";      // Default prefix for inductor IDs
    static constexpr const char* stamping_id = "I";     // Prefix for current variable in extraVarId_map
    static constexpr const char* type = "Inductor";     // Component type name for display
    
protected:
    int vc_id;          // Index for the inductor current variable in MNA system
    double inductance;  // Inductance value in Henries (H)
    double current;     // Computed current through the inductor in Amperes
    
public:
    /**
     * @brief Constructs an inductor with specified ID and inductance.
     * @param id Unique identifier (e.g., "L1", "L2").
     * @param ni Positive terminal node.
     * @param nj Negative terminal node.
     * @param l Inductance in Henries (default: 0).
     * 
     * @note Automatically allocates an extra variable ID (vc_id) for the inductor current.
     */
    Inductor(const std::string& id, Node* ni, Node* nj, double l = 0);
    
    /**
     * @brief Returns voltage drop across the inductor.
     * @return Always 0.0 V in DC analysis (short circuit behavior).
     */
    virtual double get_voltage_drop() override;
    
    /**
     * @brief Returns the computed current through the inductor.
     * @return Current in Amperes (set by deploy_dc_solution).
     */
    virtual double get_current() override;
    
    /**
     * @brief Generates MNA contributions for DC short circuit model.
     * @return Component_contribution with voltage source-like stamps (V=0).
     */
    virtual Component_contribution get_contribution() override;

    /**
     * @brief Gets the extra variable index for inductor current.
     * @return Index in the MNA system solution vector.
     */
    int get_vc_id() const { return vc_id; }
    
    /**
     * @brief Sets the inductor current (called after solving MNA system).
     * @param i Current value in Amperes.
     */
    void set_current(double i) { current = i; }

    /**
     * @brief Prints inductor information.
     * @param os Output stream (default: std::cout).
     * 
     * Format: "L(id)  node+ node-  value µH"
     */
    void print(std::ostream& os) const override;
};

#endif