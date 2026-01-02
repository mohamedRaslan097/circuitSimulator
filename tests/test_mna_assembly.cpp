#include "Include.h"
#include <cmath>
#include <sstream>
#include <vector>
#include <fstream>
#include <chrono>

const double TOLERANCE = 1e-6;

bool doubles_equal(double a, double b) {
    return std::fabs(a - b) < TOLERANCE;
}

// Helper to parse MNA output and extract matrix values
std::map<std::pair<std::string, std::string>, double> parse_mna_matrix(const std::string& output) {
    std::map<std::pair<std::string, std::string>, double> matrix;
    std::istringstream iss(output);
    std::string line;
    std::vector<std::string> headers;
    
    // Find header line
    while(std::getline(iss, line)) {
        if(line.find("|") != std::string::npos && line.find("RHS") != std::string::npos) {
            std::istringstream header_stream(line);
            std::string token;
            while(header_stream >> token) {
                if(token != "|" && token != "RHS") {
                    headers.push_back(token);
                }
            }
            break;
        }
    }
    
    // Parse matrix rows
    while(std::getline(iss, line)) {
        if(line.find("[") != std::string::npos) {
            std::string row_name;
            std::istringstream line_stream(line);
            line_stream >> row_name;
            
            std::string bracket;
            line_stream >> bracket; // Skip '['
            
            for(size_t i = 0; i < headers.size(); i++) {
                double value;
                if(line_stream >> value) {
                    matrix[{row_name, headers[i]}] = value;
                }
            }
        }
    }
    
    return matrix;
}

std::map<std::string, double> parse_mna_vector(const std::string& output) {
    std::map<std::string, double> vector;
    std::istringstream iss(output);
    std::string line;
    
    // Skip to matrix data
    while(std::getline(iss, line)) {
        if(line.find("[") != std::string::npos && line.find("]") != std::string::npos) {
            std::string row_name;
            std::istringstream line_stream(line);
            line_stream >> row_name;
            
            // Find the RHS value (last value between brackets)
            size_t last_bracket = line.rfind("[");
            if(last_bracket != std::string::npos) {
                std::string rhs_part = line.substr(last_bracket + 1);
                std::istringstream rhs_stream(rhs_part);
                double value;
                if(rhs_stream >> value) {
                    vector[row_name] = value;
                }
            }
        }
    }
    
    return vector;
}

bool test_simple_resistor_divider() {
    // V1: 10V between node 1 and 0
    // R1: 1k between node 1 and 2
    // R2: 1k between node 2 and 0
    Circuit circuit;
    circuit.parse_netlist("tests/test_netlists/voltage_divider.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream oss;
    circuit.display_MNA_system(oss);
    std::string output = oss.str();
    
    auto matrix = parse_mna_matrix(output);
    auto vector = parse_mna_vector(output);
    
    // R1 contributes: G11 += 1/1000, G12 -= 1/1000, G21 -= 1/1000, G22 += 1/1000
    // R2 contributes: G22 += 1/1000
    bool pass = doubles_equal(matrix[{"1", "1"}], 0.001) &&
                doubles_equal(matrix[{"1", "2"}], -0.001) &&
                doubles_equal(matrix[{"2", "1"}], -0.001) &&
                doubles_equal(matrix[{"2", "2"}], 0.002) &&
                doubles_equal(matrix[{"1", "IV1"}], 1.0) &&
                doubles_equal(matrix[{"IV1", "1"}], 1.0);
    
    pass = pass && doubles_equal(vector["1"], -10.0) &&
           doubles_equal(vector["2"], 0.0) &&
           doubles_equal(vector["IV1"], 0.0);
    
    return pass;
}

bool test_current_source_circuit() {
    // I1: 1A from 0 to node 1
    // R1: 1k between node 1 and 0
    Circuit circuit;
    circuit.parse_netlist("tests/test_netlists/current_source.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream oss;
    circuit.display_MNA_system(oss);
    std::string output = oss.str();
    
    auto matrix = parse_mna_matrix(output);
    auto vector = parse_mna_vector(output);
    
    // Check conductance
    bool pass = doubles_equal(matrix[{"1", "1"}], 0.001);
    
    // Check RHS (current source adds to RHS)
    pass = pass && doubles_equal(vector["1"], 1.0);
    
    return pass;
}

bool test_multiple_voltage_sources() {
    // V1: 10V between node 1 and 0
    // V2: 5V between node 2 and 0
    // R1: 1k between node 1 and 2
    Circuit circuit;
    circuit.parse_netlist("tests/test_netlists/multiple_vsources.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream oss;
    circuit.display_MNA_system(oss);
    std::string output = oss.str();
    
    auto matrix = parse_mna_matrix(output);
    auto vector = parse_mna_vector(output);
    
    // Check that we have both voltage source currents
    bool pass = doubles_equal(matrix[{"1", "IV1"}], 1.0) &&
                doubles_equal(matrix[{"IV1", "1"}], 1.0) &&
                doubles_equal(matrix[{"2", "IV2"}], 1.0) &&
                doubles_equal(matrix[{"IV2", "2"}], 1.0);
    
    // Check RHS for voltage sources
    pass = pass && doubles_equal(vector["1"], -10.0) &&
           doubles_equal(vector["2"], -5.0);
    
    return pass;
}

bool test_parallel_resistors() {
    // V1: 10V between node 1 and 0
    // R1: 1k between node 1 and 0
    // R2: 1k between node 1 and 0
    Circuit circuit;
    circuit.parse_netlist("tests/test_netlists/parallel_resistors.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream oss;
    circuit.display_MNA_system(oss);
    std::string output = oss.str();
    
    auto matrix = parse_mna_matrix(output);
    
    // Parallel resistors: G_eq = G1 + G2 = 0.001 + 0.001 = 0.002
    bool pass = doubles_equal(matrix[{"1", "1"}], 0.002);
    
    return pass;
}

bool test_series_resistors() {
    // V1: 10V between node 1 and 0
    // R1: 500 ohm between node 1 and 2
    // R2: 500 ohm between node 2 and 0
    Circuit circuit;
    circuit.parse_netlist("tests/test_netlists/series_resistors.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream oss;
    circuit.display_MNA_system(oss);
    std::string output = oss.str();
    
    auto matrix = parse_mna_matrix(output);
    
    // For node 1: G11 = 1/500 = 0.002, G12 = -1/500 = -0.002
    // For node 2: G21 = -1/500 = -0.002, G22 = 1/500 + 1/500 = 0.004
    bool pass = doubles_equal(matrix[{"1", "1"}], 0.002) &&
                doubles_equal(matrix[{"1", "2"}], -0.002) &&
                doubles_equal(matrix[{"2", "1"}], -0.002) &&
                doubles_equal(matrix[{"2", "2"}], 0.004);
    
    return pass;
}

bool test_ground_node_excluded() {
    // Test that ground node "0" is not in the MNA matrix
    Circuit circuit;
    circuit.parse_netlist("tests/test_netlists/current_source.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream oss;
    circuit.display_MNA_system(oss);
    std::string output = oss.str();
    
    auto matrix = parse_mna_matrix(output);
    
    // Check that no entries have "0" as row or column
    bool pass = true;
    for(const auto& entry : matrix) {
        if(entry.first.first == "0" || entry.first.second == "0") {
            pass = false;
            break;
        }
    }
    
    return pass;
}

bool test_wheatstone_bridge() {
    // Full Wheatstone bridge circuit from netlist
    Circuit circuit;
    circuit.parse_netlist("main/netlist.txt");
    circuit.assemble_MNA_system();
    
    std::ostringstream oss;
    circuit.display_MNA_system(oss);
    std::string output = oss.str();
    
    auto matrix = parse_mna_matrix(output);
    auto vector = parse_mna_vector(output);
    
    // Verify some key matrix entries
    bool pass = doubles_equal(matrix[{"1", "1"}], 0.002) &&  // 1/1000 + 1/1000
                doubles_equal(matrix[{"2", "2"}], 0.012) &&  // 1/1000 + 1/1000 + 1/100
                doubles_equal(matrix[{"2", "3"}], -0.01) &&  // -1/100
                doubles_equal(matrix[{"1", "IV1"}], 1.0);
    
    // Check RHS
    pass = pass && doubles_equal(vector["1"], -10.0) &&
           doubles_equal(vector["4"], 1.0);
    
    return pass;
}

bool test_display_output() {
    // Test that display_MNA_system produces output without crashing
    Circuit circuit;
    circuit.parse_netlist("tests/test_netlists/voltage_divider.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream oss;
    circuit.display_MNA_system(oss);
    
    std::string output = oss.str();
    
    // Check that output contains expected elements
    bool pass = output.find("Circuit MNA System") != std::string::npos &&
                output.find("RHS") != std::string::npos &&
                output.find("1") != std::string::npos &&
                output.find("IV1") != std::string::npos;
    
    // Check that ground node doesn't appear in the matrix rows
    // (note: it might appear in the RHS value like "1" or "-10" but not as row label)
    
    return pass;
}

bool test_performance_small_circuit() {
    // Performance test: measure assembly time for small circuit
    Circuit circuit;
    circuit.parse_netlist("tests/test_netlists/voltage_divider.net");
    
    auto start = std::chrono::high_resolution_clock::now();
    circuit.assemble_MNA_system();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Assembly should complete in under 1ms for small circuits
    bool pass = duration.count() < 1;
    std::cout << "      Small circuit assembly: " << duration.count() << " ms" << std::endl;
    if(!pass) {
        std::cout << "      Assembly took " << duration.count() << " ms (expected < 1 ms)" << std::endl;
    }
    
    return pass;
}

bool test_performance_large_circuit() {
    // Performance test: measure assembly time for large grid circuit (~1800 components)
    Circuit circuit;
    circuit.parse_netlist("tests/test_netlists/large_grid.net");
    
    auto start = std::chrono::high_resolution_clock::now();
    circuit.assemble_MNA_system();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Assembly should complete in under 500ms for ~1800 component circuit
    bool pass = duration.count() < 10;
    
    std::cout << "      Large circuit (1802 components) assembly: " << duration.count() << " ms";
    if(!pass) {
        std::cout << " (expected < 10 ms)";
    }
    std::cout << std::endl;
    
    // Verify correctness: check that matrix was actually assembled
    std::ostringstream oss;
    circuit.display_MNA_system(oss);
    std::string output = oss.str();
    
    auto matrix = parse_mna_matrix(output);
    auto vector = parse_mna_vector(output);
    
    // Verify matrix is not empty
    bool correctness_pass = !matrix.empty() && !vector.empty();
    
    // For a grid circuit, interior nodes should have conductance = 4/R (connected to 4 neighbors)
    // For 100 ohm resistors, that's 0.04
    // Check a few interior nodes exist and have reasonable values
    if(correctness_pass) {
        // Node 505 should be an interior node (row 5, col 5)
        double g_505 = matrix[{"505", "505"}];
        correctness_pass = correctness_pass && (g_505 > 0.03 && g_505 < 0.05);
    }
    
    // Check that the voltage source appears in the matrix
    if(correctness_pass) {
        correctness_pass = correctness_pass && (matrix[{"101", "IV1"}] != 0.0 || matrix[{"1", "IV1"}] != 0.0);
    }
    
    if(!correctness_pass) {
        std::cout << "      Matrix verification FAILED - output appears incorrect" << std::endl;
    }
    
    return pass && correctness_pass;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   MNA Assembly Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    int passed = 0;
    int total = 0;
    
    total++;
    if(test_simple_resistor_divider()) {
        std::cout << "[PASS] Simple resistor divider" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Simple resistor divider" << std::endl;
    }
    
    total++;
    if(test_current_source_circuit()) {
        std::cout << "[PASS] Current source circuit" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Current source circuit" << std::endl;
    }
    
    total++;
    if(test_multiple_voltage_sources()) {
        std::cout << "[PASS] Multiple voltage sources" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Multiple voltage sources" << std::endl;
    }
    
    total++;
    if(test_parallel_resistors()) {
        std::cout << "[PASS] Parallel resistors" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Parallel resistors" << std::endl;
    }
    
    total++;
    if(test_series_resistors()) {
        std::cout << "[PASS] Series resistors" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Series resistors" << std::endl;
    }
    
    total++;
    if(test_ground_node_excluded()) {
        std::cout << "[PASS] Ground node excluded from matrix" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Ground node excluded from matrix" << std::endl;
    }
    
    total++;
    if(test_wheatstone_bridge()) {
        std::cout << "[PASS] Wheatstone bridge circuit" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Wheatstone bridge circuit" << std::endl;
    }
    
    total++;
    if(test_display_output()) {
        std::cout << "[PASS] Display MNA system output" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Display MNA system output" << std::endl;
    }
    
    total++;
    if(test_performance_small_circuit()) {
        std::cout << "[PASS] Performance: small circuit assembly" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Performance: small circuit assembly" << std::endl;
    }
    
    total++;
    if(test_performance_large_circuit()) {
        std::cout << "[PASS] Performance: large circuit (1802 components)" << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] Performance: large circuit (1802 components)" << std::endl;
    }
    
    std::cout << "========================================" << std::endl;
    std::cout << "   Test Results: " << passed << "/" << total << " passed" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (passed == total) ? 0 : 1;
}
