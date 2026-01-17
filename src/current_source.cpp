#include "current_source.h"

Current_source::Current_source(const std::string& id, Node* ni, Node* nj, double c) : Component(id, ni, nj), current(c) {}

double Current_source::get_voltage_drop() {
    if(!Node::valid)
        throw std::runtime_error("Node voltages are not valid.");
    return ni->voltage - nj->voltage;
}

double Current_source::get_current() {return current;}

void Current_source::print(std::ostream& os) const {
    os << std::left << std::setw(10) << "I(" + componentId + ")"
       << std::setw(6) << ni->name 
       << std::setw(6) << nj->name 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << current << " A" << std::endl;
}

Component_contribution Current_source::get_contribution(){
    Component_contribution contribution;
    if(ni->id != 0){
        contribution.stampVector(ni->id, -current);
    }
    if(nj->id != 0){
        contribution.stampVector(nj->id, current);
    }
    return contribution;
}