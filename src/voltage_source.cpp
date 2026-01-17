#include "voltage_source.h"

Voltage_source::Voltage_source(const std::string& id, Node* ni, Node* nj, double v) : Component(id, ni, nj),vc_id(Node::node_count++), voltage(v), current(0) {}

double Voltage_source::get_voltage_drop() {return voltage;}

double Voltage_source::get_current() {return current;}

void Voltage_source::print(std::ostream& os) const {
    os << std::left << std::setw(10) << "V(" + componentId + ")"
       << std::setw(6) << ni->name 
       << std::setw(6) << nj->name 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << voltage << " V" << std::endl;
}

Component_contribution Voltage_source::get_contribution(){
    Component_contribution contribution;
    if(ni->id != 0){
        contribution.stampMatrix(ni->id, vc_id, 1.0);
        contribution.stampMatrix(vc_id, ni->id, 1.0);
    }
    if(nj->id != 0){
        contribution.stampMatrix(nj->id, vc_id, -1.0);
        contribution.stampMatrix(vc_id, nj->id, -1.0);
    }
    contribution.stampVector(vc_id, voltage);
    return contribution;
}