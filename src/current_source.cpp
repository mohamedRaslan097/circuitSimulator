#include "current_source.h"

int Current_source::id_counter = 0;

Current_source::Current_source(const std::string& id, Node* ni, Node* nj, double c) : Component(id, ni, nj), current(c) {}

Current_source::Current_source(Node* ni, Node* nj, double c) : Current_source(default_id+std::to_string(id_counter++), ni, nj, c) {}

double Current_source::get_voltage_drop() {
    if(!Node::valid)
        throw std::runtime_error("Node voltages are not valid.");
    return ni->voltage - nj->voltage;
}

double Current_source::get_current() {return current;}

void Current_source::print(std::ostream& os) const {
    os << std::left << std::setw(10) << "I(" + componentId + ")"
       << std::setw(6) << ni->id 
       << std::setw(6) << nj->id 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << current << " A" << std::endl;
}