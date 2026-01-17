#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "Component.h"

class Capacitor : public Component {
public:
    static constexpr const char* default_id = "C";
    static constexpr const char* type = "Capacitor";
protected:
    double capacitance;
public:
    Capacitor(const std::string& id, Node* ni, Node* nj, double c);

    
    virtual double get_voltage_drop() override;
    virtual double get_current() override;
    virtual Component_contribution get_contribution() override;

    virtual void print(std::ostream& os = std::cout) const override;
};

#endif