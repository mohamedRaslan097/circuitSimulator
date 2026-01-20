#include "../include/Include.h"
#include <cassert>
#include <fstream>
#include <sstream>

using namespace std;

// QA Test Suite: Netlist Parsing
// Testing netlist file parsing behavior

const string TEST_DIR = "tests/test_netlists/";

void test_basic_netlist_parsing() {
    string filename = "basic.net";
    
    Circuit circuit;
    circuit.parse_netlist(TEST_DIR + filename);
    
    ostringstream oss;
    circuit.print(oss);
    
    string output = oss.str();
    
    assert(output.find("Simple Circuit") != string::npos);
    assert(output.find("V1") != string::npos);
    assert(output.find("R1") != string::npos);
    
    cout << "[PASS] Basic netlist parsing" << endl;
}

void test_single_word_comment_lines() {
    string filename = "single_comment.net";
    
    Circuit circuit;
    
    try {
        circuit.parse_netlist(TEST_DIR + filename);
        
        ostringstream oss;
        circuit.print(oss);
        string output = oss.str();
        
        int v_count = 0, r_count = 0;
        size_t pos = 0;
        while((pos = output.find("V(V", pos)) != string::npos) { v_count++; pos++; }
        pos = 0;
        while((pos = output.find("R(R", pos)) != string::npos) { r_count++; pos++; }
        
        if(v_count != 1 || r_count != 1) {
            cout << "[ISSUE] Single-word comment: Expected 1V 1R, Found " << v_count << "V " << r_count << "R" << endl;
        } else {
            cout << "[PASS] Single-word comment lines" << endl;
        }
        
    } catch(const exception& e) {
        cout << "[ISSUE] Single-word comment exception: " << e.what() << endl;
    }
}

void test_multi_word_comment_lines() {
    string filename = "multi_comment.net";
    
    Circuit circuit;
    
    try {
        circuit.parse_netlist(TEST_DIR + filename);
        
        ostringstream oss;
        circuit.print(oss);
        string output = oss.str();
        
        int v_count = 0, r_count = 0;
        size_t pos = 0;
        while((pos = output.find("V(V", pos)) != string::npos) { v_count++; pos++; }
        pos = 0;
        while((pos = output.find("R(R", pos)) != string::npos) { r_count++; pos++; }
        
        if(v_count != 1 || r_count != 1) {
            cout << "[ISSUE] Multi-word comment: Expected 1V 1R, Found " << v_count << "V " << r_count << "R" << endl;
        } else {
            cout << "[PASS] Multi-word comment lines" << endl;
        }
        
    } catch(const exception& e) {
        cout << "[ISSUE] Multi-word comment exception: " << e.what() << endl;
    }
}

void test_multiple_consecutive_comments() {
    string filename = "consecutive_comments.net";
    
    Circuit circuit;
    
    try {
        circuit.parse_netlist(TEST_DIR + filename);
        
        ostringstream oss;
        circuit.print(oss);
        string output = oss.str();
        
        int v_count = 0, r_count = 0;
        size_t pos = 0;
        while((pos = output.find("V(V", pos)) != string::npos) { v_count++; pos++; }
        pos = 0;
        while((pos = output.find("R(R", pos)) != string::npos) { r_count++; pos++; }
        
        if(v_count != 1 || r_count != 1) {
            cout << "[ISSUE] Consecutive comments: Expected 1V 1R, Found " << v_count << "V " << r_count << "R" << endl;
        } else {
            cout << "[PASS] Multiple consecutive comments" << endl;
        }
        
    } catch(const exception& e) {
        cout << "[ISSUE] Consecutive comments exception: " << e.what() << endl;
    }
}

void test_inline_comments() {
    string filename = "inline_comments.net";
    
    Circuit circuit;
    
    try {
        circuit.parse_netlist(TEST_DIR + filename);
        
        ostringstream oss;
        circuit.print(oss);
        string output = oss.str();
        
        int v_count = 0, r_count = 0;
        size_t pos = 0;
        while((pos = output.find("V(V", pos)) != string::npos) { v_count++; pos++; }
        pos = 0;
        while((pos = output.find("R(R", pos)) != string::npos) { r_count++; pos++; }
        
        if(v_count != 1 || r_count != 1) {
            cout << "[ISSUE] Inline comments: Expected 1V 1R, Found " << v_count << "V " << r_count << "R" << endl;
        } else {
            cout << "[PASS] Inline comments" << endl;
        }
        
    } catch(const exception& e) {
        cout << "[ISSUE] Inline comments exception: " << e.what() << endl;
    }
}

void test_empty_lines_in_netlist() {
    string filename = "empty_lines.net";
    
    Circuit circuit;
    
    try {
        circuit.parse_netlist(TEST_DIR + filename);
        
        ostringstream oss;
        circuit.print(oss);
        string output = oss.str();
        
        int v_count = 0, r_count = 0;
        size_t pos = 0;
        while((pos = output.find("V(V", pos)) != string::npos) { v_count++; pos++; }
        pos = 0;
        while((pos = output.find("R(R", pos)) != string::npos) { r_count++; pos++; }
        
        if(v_count != 1 || r_count != 1) {
            cout << "[ISSUE] Empty lines: Expected 1V 1R, Found " << v_count << "V " << r_count << "R" << endl;
        } else {
            cout << "[PASS] Empty lines handling" << endl;
        }
        
    } catch(const exception& e) {
        cout << "[ISSUE] Empty lines exception: " << e.what() << endl;
    }
}

void test_mixed_case_component_types() {
    string filename = "mixed_case.net";
    
    Circuit circuit;
    circuit.parse_netlist(TEST_DIR + filename);
    
    ostringstream oss;
    circuit.print(oss);
    string output = oss.str();
    
    assert(output.find("v1") != string::npos || output.find("V(v1)") != string::npos);
    assert(output.find("r1") != string::npos || output.find("R(r1)") != string::npos);
    assert(output.find("i1") != string::npos || output.find("I(i1)") != string::npos);
    
    cout << "[PASS] Mixed case component types" << endl;
}

void test_component_value_precision() {
    string filename = "value_precision.net";
    
    Circuit circuit;
    circuit.parse_netlist(TEST_DIR + filename);
    
    ostringstream oss;
    circuit.print(oss);
    string output = oss.str();
    
    assert(output.find("3.3") != string::npos);      // Voltage source: 3.3 V
    assert(output.find("4.7") != string::npos);      // Resistor: 4700 Ohms = 4.7 kOhm
    
    cout << "[PASS] Component value precision" << endl;
}

void test_existing_netlist_file() {
    Circuit circuit;
    
    string filename = "netlist.txt";
    circuit.parse_netlist(TEST_DIR + filename);
    
    ostringstream oss;
    circuit.print(oss);
    string output = oss.str();
    
    assert(output.find("Wheatstone Bridge") != string::npos);
    assert(output.find("V1") != string::npos);
    assert(output.find("R1") != string::npos);
    assert(output.find("R2") != string::npos);
    assert(output.find("R3") != string::npos);
    assert(output.find("R4") != string::npos);
    assert(output.find("R5") != string::npos);
    
    cout << "[PASS] Existing netlist.txt parsing" << endl;
}

void test_existing_comment_netlist_file() {
    Circuit circuit;
    
    try {
        circuit.parse_netlist(TEST_DIR + "test_netlist_comments.txt");
        
        ostringstream oss;
        circuit.print(oss);
        string output = oss.str();
        
        int v_count = 0, r_count = 0;
        size_t pos = 0;
        while((pos = output.find("V(V", pos)) != string::npos) { v_count++; pos++; }
        pos = 0;
        while((pos = output.find("R(R", pos)) != string::npos) { r_count++; pos++; }
        
        if(v_count != 1 || r_count != 2) {
            cout << "[ISSUE] Existing comment netlist: Expected 1V 2R, Found " << v_count << "V " << r_count << "R" << endl;
        } else {
            cout << "[PASS] Existing comment netlist parsing" << endl;
        }
        
    } catch(const exception& e) {
        cout << "[ISSUE] Existing comment netlist exception: " << e.what() << endl;
    }
}

int main() {
    cout << "\n========================================" << endl;
    cout << "   QA: Netlist Parsing Test Suite" << endl;
    cout << "========================================\n" << endl;
    
    test_basic_netlist_parsing();
    test_single_word_comment_lines();
    test_multi_word_comment_lines();
    test_multiple_consecutive_comments();
    test_inline_comments();
    test_empty_lines_in_netlist();
    test_mixed_case_component_types();
    test_component_value_precision();
    test_existing_netlist_file();
    test_existing_comment_netlist_file();
    
    cout << "\n========================================" << endl;
    cout << "   Test Suite Complete" << endl;
    cout << "========================================\n" << endl;
    
    return 0;
}
