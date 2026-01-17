#ifndef ResISTOR_H
#define ResISTOR_H

#include "component.h"

class Resistor : public Component{
public:
    static constexpr const char* default_id = "R";
    static constexpr const char* type = "Resistor";
protected:
    double resistance;
public:
    Resistor( const std::string& id, Node* ni, Node* nj, double r = 0);
    Resistor(Node* ni, Node* nj, double r);
    virtual double get_voltage_drop() override;
    virtual double get_current() override;
    virtual Component_contribution get_contribution() override;
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif