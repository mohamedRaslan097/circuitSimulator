/**
 * @file component.h
 * @brief Abstract base class for all circuit components.
 * 
 * Defines the common interface and properties shared by all electrical
 * components (resistors, capacitors, inductors, voltage sources, current sources).
 * Each component connects two nodes and contributes to the MNA system.
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include "I_printable.h"
#include "node.h"
#include "component_contribution.h"

/**
 * @class Component
 * @brief Abstract base class for two-terminal electrical components.
 * 
 * All circuit components inherit from this class and must implement:
 * - get_voltage_drop(): Returns the voltage across the component
 * - get_current(): Returns the current through the component
 * - get_contribution(): Returns MNA matrix/vector stamps
 * 
 * Components are defined by their connection between two nodes (ni and nj),
 * where ni is the positive terminal and nj is the negative terminal.
 * Current flows from ni to nj (conventional current direction).
 * 
 * @note This is an abstract class and cannot be instantiated directly.
 * 
 * @see Resistor, Capacitor, Inductor, Voltage_source, Current_source
 */
class Component : public I_Printable {
protected:
    Node* ni;                   // Pointer to the positive terminal node
    Node* nj;                   // Pointer to the negative terminal node
    std::string componentId;    // Unique identifier for the component (e.g., "R1", "V1")
    
public:
    /**
     * @brief Constructs a component between two nodes.
     * @param id Unique identifier string (e.g., "R1", "V1", "C1").
     * @param node_i Pointer to the positive terminal node.
     * @param node_j Pointer to the negative terminal node.
     */
    Component(const std::string& id, Node* node_i, Node* node_j);
    
    /**
     * @brief Copy constructor.
     * @param src Source component to copy from.
     * @note Performs shallow copy of node pointers.
     */
    Component(const Component& src);
    
    /**
     * @brief Move constructor.
     * @param src Source component to move from.
     * @note Source node pointers are set to nullptr after move.
     */
    Component(Component&& src);
    
    /**
     * @brief Gets the voltage drop across the component.
     * @return Voltage difference (Voltage drop) in Volts.
     * @throws std::runtime_error if node voltages are not valid.
     * 
     * @note For voltage sources, returns the source voltage value.
     */
    virtual double get_voltage_drop() = 0;
    
    /**
     * @brief Gets the current flowing through the component.
     * @return Current in Amperes (positive from ni to nj).
     * @throws std::runtime_error if circuit has not been solved.
     * 
     * @note For current sources, returns the source current value.
     */
    virtual double get_current() = 0;
    
    /**
     * @brief Generates MNA contributions for this component.
     * @return Component_contribution containing matrix and vector stamps.
     * 
     * Each component type has a specific stamping pattern:
     * - Resistors: Conductance stamps to matrix
     * - Voltage sources: ±1 stamps and voltage to vector (with extra variable)
     * - Current sources: Current stamps to vector only
     * - Inductors: Short circuit in DC (±1 stamps like voltage source)
     * - Capacitors: Open circuit in DC (no contribution)
     */
    virtual Component_contribution get_contribution() = 0;
    
    /**
     * @brief Destructor.
     * @note Does not delete nodes (owned by Circuit class).
     */
    ~Component();
};

#endif