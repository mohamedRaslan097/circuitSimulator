#ifndef INDUCTOR_H
#define INDUCTOR_H

#include "Component.h"

class Inductor : public Component {
public:
    static constexpr const char* default_id = "L";
    static constexpr const char* stamping_id = "I";
    static constexpr const char* type = "Inductor";
protected:
    int vc_id;
    double inductance;
    double current;
public:
    Inductor(const std::string& id, Node* ni, Node* nj, double l = 0);
    
    virtual double get_voltage_drop() override;
    virtual double get_current() override;
    virtual Component_contribution get_contribution() override;

    int get_vc_id() const { return vc_id; }
    
    void set_current(double i) { current = i; }

    void print(std::ostream& os) const override;
};

#endif