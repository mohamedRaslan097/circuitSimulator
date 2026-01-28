/**
 * @file capacitor.h
 * @brief Capacitor component class for the circuit simulator.
 * 
 * Implements a linear capacitor with capacitance C.
 * In DC analysis, capacitors act as open circuits (no current flow).
 */

#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "Component.h"

/**
 * @class Capacitor
 * @brief Represents a linear capacitor in the circuit.
 * 
 * A capacitor is a passive two-terminal component that stores energy
 * in an electric field. The current-voltage relationship is:
 * I = C * dV/dt
 * 
 * **DC Analysis Behavior:**
 * In steady-state DC analysis, dV/dt = 0, so capacitors behave as
 * open circuits with zero current flow. The capacitor contributes
 * nothing to the MNA system in DC analysis.
 * 
 * **AC Analysis Behavior:**
 * In AC analysis, the capacitor contributes an impedance of 1/(jωC),
 * where ω = 2πf.
 * 
 * @see Component, Component_contribution
 * 
 * @example
 * @code
 * Node* n1 = new Node("1");
 * Node* n2 = new Node("2");
 * Capacitor c("C1", n1, n2, 1e-6);  // 1µF capacitor
 * @endcode
 */
class Capacitor : public Ac_component {
public:
    static constexpr const char* default_id = "C";      // Default prefix for capacitor IDs
    static constexpr const char* type = "Capacitor";    // Component type name for display
    
protected:
    double capacitance;                 // Capacitance value in Farads (F)
    std::complex<double> admittance;    // Admittance equivalent for AC analysis (jωC)
    
public:
    /**
     * @brief Constructs a capacitor with specified ID and capacitance.
     * @param id Unique identifier (e.g., "C1", "C2").
     * @param ni Positive terminal node.
     * @param nj Negative terminal node.
     * @param c Capacitance in Farads.
     */
    Capacitor(const std::string& id, Node* ni, Node* nj, double c);

    /**
     * @brief Calculates voltage drop across the capacitor.
     * @return Voltage drop in Volts.
     * @throws std::runtime_error if node voltages are not valid.
     */
    virtual double get_voltage_drop() override;
    
    /**
     * @brief Returns current through the capacitor.
     * @return Always 0.0 A in DC analysis (open circuit behavior).
     */
    virtual double get_current() override;
    
    /**
     * @brief Generates MNA contributions (empty for DC analysis).
     * @return Empty Component_contribution (capacitor is open circuit in DC).
     */
    virtual Component_contribution<double> get_contribution() override;

    /**
     * @brief Generates AC MNA contributions for the capacitor.
     * @param frequency AC analysis frequency in Hertz.
     * @return Component_contribution with capacitive reactance stamps.
     * 
     * In AC analysis, the capacitor contributes an impedance of 1/(jωC),
     * where ω = 2πf. The stamping pattern differs from DC analysis.
     */
    virtual Component_contribution<std::complex<double>> get_ac_contribution(double frequency) override;

    /**
     * @brief Prints capacitor information.
     * @param os Output stream (default: std::cout).
     * 
     * Format: "C(id)  node+ node-  value nF"
     */
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif