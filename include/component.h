#ifndef COMPONENT_H
#define COMPONENT_H

#include "I_printable.h"
#include "node.h"
#include "component_contribution.h"

class Component : public I_Printable {
protected:
    Node* ni;
    Node* nj;
    std::string componentId;
public:
    Component(const std::string& id, Node* node_i, Node* node_j);
    Component(const Component& src);
    Component(Component&& src);
    virtual double get_voltage_drop() = 0;
    virtual double get_current() = 0;
    virtual Component_contribution get_contribution() = 0;
    ~Component();
};

#endif