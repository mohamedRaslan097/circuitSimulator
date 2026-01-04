#ifndef VOLTAGE_SOURCE_H
#define VOLTAGE_SOURCE_H

#include "Component.h"

class Voltage_source : public Component {
private:
    static int id_counter;
public:
    static constexpr const char* default_id = "V";
    static constexpr const char* stamping_id = "I";
    static constexpr const char* type = "Voltage Source";
protected:
    int vc_id;
    double voltage;
    double current;
public:
    Voltage_source(const std::string& id, Node* ni, Node* nj, double v = 0);
    Voltage_source(Node* ni, Node* nj, double v);
    virtual double get_voltage_drop() override;
    virtual double get_current() override;
    virtual Component_contribution get_contribution() override;
    virtual void print(std::ostream& os = std::cout) const override;
    int get_vc_id() const { return vc_id; }
    void set_current(double i) { current = i; }
};

#endif