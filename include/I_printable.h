/**
 * @file I_printable.h
 * @brief Interface for printable objects in the circuit simulator.
 * 
 * Provides a common interface for all classes that need to support
 * formatted output to streams. This enables polymorphic printing
 * through the overloaded stream insertion operator.
 */

#ifndef I_PRINTABLE_H
#define I_PRINTABLE_H

#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <fstream>

/**
 * @class I_Printable
 * @brief Abstract base class providing a printable interface.
 * 
 * This interface allows derived classes to define custom print behavior
 * while supporting output through the standard stream insertion operator (<<).
 * All circuit components, nodes, and other printable entities inherit from this class.
 * 
 * @note This is a pure virtual class and cannot be instantiated directly.
 * 
 * @example
 * @code
 * // Example usage with derived class:
 * Node node("N1");
 * std::cout << node;  // Uses overloaded << operator
 * node.print(std::cerr);  // Direct print to stderr
 * @endcode
 */
class I_Printable {
public:
    /**
     * @brief Friend function to enable stream insertion operator.
     * @param os The output stream to write to.
     * @param obj The I_Printable object to print.
     * @return Reference to the output stream for chaining.
     */
    friend std::ostream& operator<<(std::ostream& os, const I_Printable& obj);
    
    /**
     * @brief Pure virtual function for custom print implementation.
     * @param os The output stream to write to (default: std::cout).
     * 
     * Derived classes must implement this method to define their
     * specific output format.
     */
    virtual void print(std::ostream& os = std::cout) const = 0;
    
    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~I_Printable() = default;
};

#endif