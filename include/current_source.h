#ifndef CURRENT_SOURCE_H
#define CURRENT_SOURCE_H

#include "component.h"

class Current_source : public Component {
public:
    static constexpr const char* default_id = "I";
    static constexpr const char* type = "Current Source";
protected:
    double current;
    public:
    Current_source(const std::string& id, Node* ni, Node* nj, double c = 0);
    Current_source(Node* ni, Node* nj, double c);
    virtual double get_voltage_drop() override;
    virtual double get_current() override;
    virtual Component_contribution get_contribution() override;
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif