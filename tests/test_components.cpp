#include "../include/Include.h"
#include <cassert>
#include <sstream>

using namespace std;

// QA Test Suite: Component Tests
// Testing component creation and functionality

void test_resistor_creation() {
    Node n1("1");
    Node n2("2");
    Resistor r1("R1", &n1, &n2, 100.0);
    
    ostringstream oss;
    r1.print(oss);
    string output = oss.str();
    assert(output.find("R(R1)") != string::npos);
    assert(output.find("100.0000 Ohms") != string::npos);
    
    cout << "[PASS] Resistor creation test passed" << endl;
}

void test_voltage_source_creation() {
    Node n1("1");
    Node n2("2");
    Voltage_source vs("V1", &n1, &n2, 12.0);
    
    assert(vs.get_voltage_drop() == 12.0);
    assert(vs.get_current() == 0.0);
    
    ostringstream oss;
    vs.print(oss);
    string output = oss.str();
    assert(output.find("V(V1)") != string::npos);
    assert(output.find("12.0000 V") != string::npos);
    
    cout << "[PASS] Voltage source creation test passed" << endl;
}

void test_current_source_creation() {
    Node n1("1");
    Node n2("2");
    Current_source cs("I1", &n1, &n2, 0.5);
    
    assert(cs.get_current() == 0.5);
    
    ostringstream oss;
    cs.print(oss);
    string output = oss.str();
    assert(output.find("I(I1)") != string::npos);
    assert(output.find("0.5000 A") != string::npos);
    
    cout << "[PASS] Current source creation test passed" << endl;
}

void test_node_creation() {
    Node n1("GND");
    n1.voltage = 0.0;
    
    Node n2("VCC");
    n2.voltage = 5.0;
    
    assert(n1.name == "GND");
    assert(n2.name == "VCC");
    assert(n1.voltage == 0.0);
    assert(n2.voltage == 5.0);
    
    assert(n1 == Node("GND"));
    assert(n1 < n2);
    
    cout << "[PASS] Node creation test passed" << endl;
}

void test_multiple_components() {
    Node n1("1");
    Node n2("2");
    Node n3("3");
    
    Resistor r1("R0",&n1, &n2, 1000.0);
    Resistor r2("R1",&n2, &n3, 2000.0);
    Voltage_source vs("V0",&n1, &n3, 9.0);
    Current_source cs("I0",&n2, &n1, 0.001);
    
    ostringstream oss;
    oss << r1 << r2 << vs << cs;
    string output = oss.str();
    
    assert(output.find("R(R0)") != string::npos);
    assert(output.find("R(R1)") != string::npos);
    assert(output.find("V(V0)") != string::npos);
    assert(output.find("I(I0)") != string::npos);
    
    cout << "[PASS] Multiple components test passed" << endl;
}

void test_component_printing_format() {
    Node n1("A");
    Node n2("B");
    
    Resistor r("R_TEST", &n1, &n2, 470.5);
    Voltage_source v("V_TEST", &n1, &n2, 3.3);
    Current_source i("I_TEST", &n1, &n2, 0.02);
    
    ostringstream oss_r, oss_v, oss_i;
    r.print(oss_r);
    v.print(oss_v);
    i.print(oss_i);
    
    assert(oss_r.str().find("R(R_TEST)") != string::npos);
    assert(oss_r.str().find("A") != string::npos);
    assert(oss_r.str().find("B") != string::npos);
    
    assert(oss_v.str().find("V(V_TEST)") != string::npos);
    assert(oss_i.str().find("I(I_TEST)") != string::npos);
    
    cout << "[PASS] Component printing format test passed" << endl;
}

int main() {
    cout << "Running component tests..." << endl;
    cout << "=========================" << endl;
    
    test_node_creation();
    test_resistor_creation();
    test_voltage_source_creation();
    test_current_source_creation();
    test_multiple_components();
    test_component_printing_format();
    
    cout << "=========================" << endl;
    cout << "All tests passed!" << endl;
    
    return 0;
}
