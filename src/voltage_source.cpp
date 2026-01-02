#include "voltage_source.h"

int Voltage_source::id_counter = 0;

Voltage_source::Voltage_source(const std::string& id, Node* ni, Node* nj, double v) : Component(id, ni, nj), voltage(v), current(0) {}

Voltage_source::Voltage_source(Node* ni, Node* nj, double v) : Voltage_source(default_id+std::to_string(id_counter++), ni, nj, v) {}

double Voltage_source::get_voltage_drop() {return voltage;}

double Voltage_source::get_current() {return current;}

void Voltage_source::print(std::ostream& os) const {
    os << std::left << std::setw(10) << "V(" + componentId + ")"
       << std::setw(6) << ni->id 
       << std::setw(6) << nj->id 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << voltage << " V" << std::endl;
}

Component_contribution Voltage_source::get_contribution(){
    Component_contribution contribution;
    std::string vc_id = "I" + componentId;
    if(ni->id != "0"){
        contribution.stampMatrix(ni->id, vc_id, 1.0);
        contribution.stampMatrix(vc_id, ni->id, 1.0);
        contribution.stampVector(ni->id, -voltage);
    }
    if(nj->id != "0"){
        contribution.stampMatrix(nj->id, vc_id, 1.0);
        contribution.stampMatrix(vc_id, nj->id, -1.0);
        contribution.stampVector(nj->id, voltage);
    }
    return contribution;
}