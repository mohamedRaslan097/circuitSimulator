#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <set>
#include "circuit.h"

// Constants
const double TOLERANCE = 1e-8;


// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

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

// ============================================================================
// TEST RUNNER CLASS
// ============================================================================

class MNATestRunner {
private:
    int passed = 0;
    int failed = 0;
    int total_tests = 0;
    std::string current_test;
    std::vector<std::string> failed_tests;

public:
    void start_test(const std::string& name) {
        current_test = name;
        total_tests++;
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "  " << name << std::endl;
        std::cout << std::string(70, '=') << std::endl;
    }

    void assert_matrix_value(const std::map<std::pair<std::string, std::string>, double>& matrix,
                            const std::string& row, const std::string& col, double expected,
                            const std::string& description = "") {
        auto key = std::make_pair(row, col);
        double actual = 0.0;
        
        if (matrix.find(key) != matrix.end()) {
            actual = matrix.at(key);
        }
        
        bool pass = doubles_equal(actual, expected);
        
        std::cout << "  A[" << std::setw(4) << row << "][" << std::setw(4) << col << "] = "
                  << std::setw(12) << std::fixed << std::setprecision(4) << actual
                  << " (expected: " << std::setw(12) << expected << ") ";
        
        if (!description.empty()) {
            std::cout << "// " << description << " ";
        }
        
        if (pass) {
            passed++;
            std::cout << "[PASS]" << std::endl;
        } else {
            failed++;
            std::cout << "[FAIL] (diff: " << std::scientific << (actual - expected) << ")" << std::endl;
            failed_tests.push_back(current_test + " - A[" + row + "][" + col + "]");
        }
    }

    void assert_rhs_value(const std::map<std::string, double>& vector,
                         const std::string& index, double expected,
                         const std::string& description = "") {
        double actual = 0.0;
        
        if (vector.find(index) != vector.end()) {
            actual = vector.at(index);
        }
        
        bool pass = doubles_equal(actual, expected);
        
        std::cout << "  b[" << std::setw(4) << index << "] = "
                  << std::setw(12) << std::fixed << std::setprecision(4) << actual
                  << " (expected: " << std::setw(12) << expected << ") ";
        
        if (!description.empty()) {
            std::cout << "// " << description << " ";
        }
        
        if (pass) {
            passed++;
            std::cout << "[PASS]" << std::endl;
        } else {
            failed++;
            std::cout << "[FAIL] (diff: " << std::scientific << (actual - expected) << ")" << std::endl;
            failed_tests.push_back(current_test + " - b[" + index + "]");
        }
    }

    void assert_true(bool condition, const std::string& message) {
        std::cout << "  " << message << ": ";
        if (condition) {
            passed++;
            std::cout << "[PASS]" << std::endl;
        } else {
            failed++;
            std::cout << "[FAIL]" << std::endl;
            failed_tests.push_back(current_test + " - " + message);
        }
    }

    void print_summary() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "                         TEST SUMMARY" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        
        std::cout << "  Total Tests:       " << total_tests << std::endl;
        std::cout << "  Assertions Passed: " << passed << std::endl;
        std::cout << "  Assertions Failed: " << failed << std::endl;
        std::cout << "  Total Assertions:  " << (passed + failed) << std::endl;
        
        if (failed > 0) {
            std::cout << "\n  Failed Assertions:" << std::endl;
            for (const auto& fail : failed_tests) {
                std::cout << "    [X] " << fail << std::endl;
            }
        }
        
        std::cout << std::string(70, '=') << std::endl;
        
        if (failed == 0) {
            std::cout << "\n  ALL TESTS PASSED!\n" << std::endl;
        } else {
            std::cout << "\n  SOME TESTS FAILED\n" << std::endl;
        }
    }
};

// ============================================================================
// TEST CASES
// ============================================================================

// TEST 1: Single Resistor
void test_single_resistor(MNATestRunner& runner) {
    runner.start_test("TEST 1: Single Resistor (R=1kohm between nodes 1-2)");
    
    /*
     * Circuit:  1 ----[R1=1000ohm]---- 2
     * 
     * Expected MNA Matrix:
     *        1         2
     *   1 [  0.001   -0.001  ]
     *   2 [ -0.001    0.001  ]
     * 
     * Expected RHS:
     *   b[1] = 0.0
     *   b[2] = 0.0
     */
    
    Circuit circuit("SingleResistor");
    std::ofstream netlist("test1.net");
    netlist << "* Single Resistor Circuit\n";
    netlist << "R1 1 2 1000\n";
    netlist.close();
    
    circuit.parse_netlist("test1.net");
    circuit.assemble_MNA_system();
    
    // Capture output
    std::ostringstream output;
    circuit.print_MNA_system(output);
    std::string mna_output = output.str();
    
    // Parse matrix and vector
    auto matrix = parse_mna_matrix(mna_output);
    auto vector = parse_mna_vector(mna_output);
    
    std::cout << "\n  Circuit Output:\n" << mna_output << std::endl;
    
    // Verify matrix values
    runner.assert_matrix_value(matrix, "1", "1",  0.001, "G11 = 1/R1");
    runner.assert_matrix_value(matrix, "1", "2", -0.001, "G12 = -1/R1");
    runner.assert_matrix_value(matrix, "2", "1", -0.001, "G21 = -1/R1");
    runner.assert_matrix_value(matrix, "2", "2",  0.001, "G22 = 1/R1");
    
    // Verify RHS values
    runner.assert_rhs_value(vector, "1", 0.0, "No source at node 1");
    runner.assert_rhs_value(vector, "2", 0.0, "No source at node 2");
    
    std::remove("test1.net");
}

// TEST 2: Series Resistors
void test_series_resistors(MNATestRunner& runner) {
    runner.start_test("TEST 2: Series Resistors (R1=1k, R2=2k)");
    
    /*
     * Circuit:  1 ----[R1=1k]---- 2 ----[R2=2k]---- 3
     * 
     * G1 = 0.001, G2 = 0.0005
     * 
     * Expected:
     *        1         2         3
     *   1 [  0.001   -0.001     0.0    ]
     *   2 [ -0.001    0.0015   -0.0005 ]
     *   3 [  0.0     -0.0005    0.0005 ]
     */
    
    Circuit circuit("SeriesResistors");
    std::ofstream netlist("test2.net");
    netlist << "R1 1 2 1000\n";
    netlist << "R2 2 3 2000\n";
    netlist.close();
    
    circuit.parse_netlist("test2.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream output;
    circuit.print_MNA_system(output);
    auto matrix = parse_mna_matrix(output.str());
    auto vector = parse_mna_vector(output.str());
    
    std::cout << "\n  Circuit Output:\n" << output.str() << std::endl;
    
    // Node 1
    runner.assert_matrix_value(matrix, "1", "1",  0.001,  "G11 = G1");
    runner.assert_matrix_value(matrix, "1", "2", -0.001,  "G12 = -G1");
    runner.assert_matrix_value(matrix, "1", "3",  0.0,    "G13 = 0");
    
    // Node 2 (middle node)
    runner.assert_matrix_value(matrix, "2", "1", -0.001,  "G21 = -G1");
    runner.assert_matrix_value(matrix, "2", "2",  0.0015, "G22 = G1+G2");
    runner.assert_matrix_value(matrix, "2", "3", -0.0005, "G23 = -G2");
    
    // Node 3
    runner.assert_matrix_value(matrix, "3", "1",  0.0,    "G31 = 0");
    runner.assert_matrix_value(matrix, "3", "2", -0.0005, "G32 = -G2");
    runner.assert_matrix_value(matrix, "3", "3",  0.0005, "G33 = G2");
    
    std::remove("test2.net");
}

// TEST 3: Parallel Resistors
void test_parallel_resistors(MNATestRunner& runner) {
    runner.start_test("TEST 3: Parallel Resistors (R1=1k || R2=2k)");
    
    /*
     * Circuit:
     *        R1=1k
     *   1 ─────────── 2
     *        R2=2k
     * 
     * G1 = 0.001, G2 = 0.0005
     * G_total = 0.0015
     * 
     * Expected:
     *        1         2
     *   1 [  0.0015  -0.0015 ]
     *   2 [ -0.0015   0.0015 ]
     */
    
    Circuit circuit("ParallelResistors");
    std::ofstream netlist("test3.net");
    netlist << "R1 1 2 1000\n";
    netlist << "R2 1 2 2000\n";
    netlist.close();
    
    circuit.parse_netlist("test3.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream output;
    circuit.print_MNA_system(output);
    auto matrix = parse_mna_matrix(output.str());
    
    std::cout << "\n  Circuit Output:\n" << output.str() << std::endl;
    
    runner.assert_matrix_value(matrix, "1", "1",  0.0015, "G11 = G1+G2");
    runner.assert_matrix_value(matrix, "1", "2", -0.0015, "G12 = -(G1+G2)");
    runner.assert_matrix_value(matrix, "2", "1", -0.0015, "G21 = -(G1+G2)");
    runner.assert_matrix_value(matrix, "2", "2",  0.0015, "G22 = G1+G2");
    
    std::remove("test3.net");
}

// TEST 4: Voltage Source
void test_voltage_source(MNATestRunner& runner) {
    runner.start_test("TEST 4: Voltage Source (V1=10V, R1=1k)");
    
    /*
     * Circuit:  0 ----[V1=10V]---- 1 ----[R1=1k]---- 2
     * 
     * Variables: V1, V2, I_V1
     * 
     * Expected (assuming extra variable is "IV1"):
     *        1       2      IV1
     *   1 [  0.001  -0.001   1.0  ] = [  0.0 ]
     *   2 [ -0.001   0.001   0.0  ] = [  0.0 ]
     * IV1 [  1.0     0.0     0.0  ] = [ 10.0 ]
     */
    
    Circuit circuit("VoltageSource");
    std::ofstream netlist("test4.net");
    netlist << "V1 0 1 10\n";
    netlist << "R1 1 2 1000\n";
    netlist.close();
    
    circuit.parse_netlist("test4.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream output;
    circuit.print_MNA_system(output);
    auto matrix = parse_mna_matrix(output.str());
    auto vector = parse_mna_vector(output.str());
    
    std::cout << "\n  Circuit Output:\n" << output.str() << std::endl;
    
    // Node equations
    runner.assert_matrix_value(matrix, "1", "1",  0.001, "Node 1: G from R1");
    runner.assert_matrix_value(matrix, "1", "2", -0.001, "Node 1: -G from R1");
    runner.assert_matrix_value(matrix, "2", "1", -0.001, "Node 2: -G from R1");
    runner.assert_matrix_value(matrix, "2", "2",  0.001, "Node 2: G from R1");
    
    // Voltage source equation (check for voltage value in RHS)
    bool found_10V = false;
    for (const auto& entry : vector) {
        if (doubles_equal(std::abs(entry.second), 10.0)) {
            found_10V = true;
            runner.assert_rhs_value(vector, entry.first, entry.second, "Voltage source: 10V");
            break;
        }
    }
    runner.assert_true(found_10V, "10V found in RHS");
    
    std::remove("test4.net");
}

// TEST 5: Current Source
void test_current_source(MNATestRunner& runner) {
    runner.start_test("TEST 5: Current Source (I1=1A, R1=1k)");
    
    /*
     * Circuit:  1 ----[I1=1A]----> 2 ----[R1=1k]---- 0
     * 
     * Expected:
     *        1       2
     *   1 [  0.0     0.0   ] = [ -1.0 ]  (current leaves)
     *   2 [  0.0     0.001 ] = [  1.0 ]  (current enters)
     */
    
    Circuit circuit("CurrentSource");
    std::ofstream netlist("test5.net");
    netlist << "I1 1 2 1\n";
    netlist << "R1 2 0 1000\n";
    netlist.close();
    
    circuit.parse_netlist("test5.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream output;
    circuit.print_MNA_system(output);
    auto matrix = parse_mna_matrix(output.str());
    auto vector = parse_mna_vector(output.str());
    
    std::cout << "\n  Circuit Output:\n" << output.str() << std::endl;
    
    // Node 2 has resistor to ground
    runner.assert_matrix_value(matrix, "2", "2", 0.001, "Node 2: G from R1");
    
    // Current source in RHS
    runner.assert_rhs_value(vector, "1", -1.0, "Node 1: current leaves");
    runner.assert_rhs_value(vector, "2",  1.0, "Node 2: current enters");
    
    std::remove("test5.net");
}

// TEST 6: Voltage Divider
void test_voltage_divider(MNATestRunner& runner) {
    runner.start_test("TEST 6: Voltage Divider (V1=10V, R1=R2=1k)");
    
    /*
     * Circuit:
     *   0 ----[V1=10V]---- 1 ----[R1=1k]---- 2 ----[R2=1k]---- 0
     * 
     * G1 = G2 = 0.001
     * 
     * Expected:
     *        1       2      IV1
     *   1 [  0.001  -0.001   1.0  ] = [  0.0 ]
     *   2 [ -0.001   0.002   0.0  ] = [  0.0 ]
     * IV1 [  1.0     0.0     0.0  ] = [ 10.0 ]
     */
    
    Circuit circuit("VoltageDivider");
    std::ofstream netlist("test6.net");
    netlist << "V1 0 1 10\n";
    netlist << "R1 1 2 1000\n";
    netlist << "R2 2 0 1000\n";
    netlist.close();
    
    circuit.parse_netlist("test6.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream output;
    circuit.print_MNA_system(output);
    auto matrix = parse_mna_matrix(output.str());
    auto vector = parse_mna_vector(output.str());
    
    std::cout << "\n  Circuit Output:\n" << output.str() << std::endl;
    
    // Node 1
    runner.assert_matrix_value(matrix, "1", "1",  0.001, "Node 1: G1");
    runner.assert_matrix_value(matrix, "1", "2", -0.001, "Node 1: -G1");
    
    // Node 2 (connects to both resistors)
    runner.assert_matrix_value(matrix, "2", "1", -0.001, "Node 2: -G1");
    runner.assert_matrix_value(matrix, "2", "2",  0.002, "Node 2: G1+G2");
    
    // Voltage source
    bool found_10V = false;
    for (const auto& entry : vector) {
        if (doubles_equal(std::abs(entry.second), 10.0)) {
            found_10V = true;
            break;
        }
    }
    runner.assert_true(found_10V, "10V voltage source in RHS");
    
    std::remove("test6.net");
}

// TEST 7: Three-Node Network
void test_three_node_network(MNATestRunner& runner) {
    runner.start_test("TEST 7: Three-Node Network with Ground Connection");
    
    /*
     * Circuit:
     *        R1=1k      R2=2k
     *   1 ─────────2─────────3
     *               │
     *              R3=3k
     *               │
     *               0
     * 
     * G1 = 0.001, G2 = 0.0005, G3 = 0.000333...
     */
    
    Circuit circuit("ThreeNodeNetwork");
    std::ofstream netlist("test7.net");
    netlist << "R1 1 2 1000\n";
    netlist << "R2 2 3 2000\n";
    netlist << "R3 2 0 3000\n";
    netlist.close();
    
    circuit.parse_netlist("test7.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream output;
    circuit.print_MNA_system(output);
    auto matrix = parse_mna_matrix(output.str());
    
    std::cout << "\n  Circuit Output:\n" << output.str() << std::endl;
    
    double G1 = 0.001;
    double G2 = 0.0005;
    double G3 = 1.0/3000.0;
    
    // Node 2 (connects to all three resistors)
    runner.assert_matrix_value(matrix, "2", "1", -G1, "Node 2: -G1");
    runner.assert_matrix_value(matrix, "2", "2",  G1+G2+G3, "Node 2: G1+G2+G3");
    runner.assert_matrix_value(matrix, "2", "3", -G2, "Node 2: -G2");
    
    std::remove("test7.net");
}

// TEST 8: Matrix Symmetry
void test_matrix_symmetry(MNATestRunner& runner) {
    runner.start_test("TEST 8: Matrix Symmetry (Passive Circuit)");
    
    /*
     * For resistor-only circuits, the conductance matrix should be symmetric
     */
    
    Circuit circuit("SymmetryTest");
    std::ofstream netlist("test8.net");
    netlist << "R1 1 2 1000\n";
    netlist << "R2 2 3 2000\n";
    netlist << "R3 1 3 3000\n";
    netlist.close();
    
    circuit.parse_netlist("test8.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream output;
    circuit.print_MNA_system(output);
    auto matrix = parse_mna_matrix(output.str());
    
    std::cout << "\n  Circuit Output:\n" << output.str() << std::endl;
    
    // Check symmetry
    bool is_symmetric = true;
    for (const auto& entry : matrix) {
        std::string row = entry.first.first;
        std::string col = entry.first.second;
        double val = entry.second;
        
        auto reverse_key = std::make_pair(col, row);
        if (matrix.find(reverse_key) != matrix.end()) {
            double reverse_val = matrix.at(reverse_key);
            if (!doubles_equal(val, reverse_val)) {
                is_symmetric = false;
                std::cout << "  Asymmetry found: A[" << row << "][" << col 
                         << "] = " << val << " but A[" << col << "][" << row 
                         << "] = " << reverse_val << std::endl;
            }
        }
    }
    
    runner.assert_true(is_symmetric, "Matrix is symmetric");
    
    std::remove("test8.net");
}

// TEST 9: Your Original Circuit
void test_original_circuit(MNATestRunner& runner) {
    runner.start_test("TEST 9: Original Circuit");
    
    /*
     * Based on your matrix values:
     * A[1][1] =  0.0020
     * A[1][2] = -0.0010
     * A[2][2] =  0.0120
     * A[2][3] = -0.0100
     * A[4][4] =  0.0020
     * b[4] = 1.0
     */
    
    Circuit circuit("OriginalCircuit");
    std::ofstream netlist("test9.net");
    // Reconstruct circuit based on matrix values
    netlist << "R1 1 2 1000\n";
    netlist << "I1 0 4 1\n";
    netlist << "R2 2 4 1000\n";
    netlist << "R3 1 3 1000\n";
    netlist << "V1 1 0 10.0\n";
    netlist << "R4 3 4 1000\n";
    netlist << "R5 2 3 100\n";
    netlist.close();
    
    circuit.parse_netlist("test9.net");
    circuit.assemble_MNA_system();
    
    std::ostringstream output;
    circuit.print_MNA_system(output);
    auto matrix = parse_mna_matrix(output.str());
    auto vector = parse_mna_vector(output.str());
    
    std::cout << "\n  Circuit Output:\n" << output.str() << std::endl;
    
    // Verify key matrix values from your original question
    runner.assert_matrix_value(matrix, "1", "1",  0.0020, "A[1][1]");
    runner.assert_matrix_value(matrix, "1", "2", -0.0010, "A[1][2]");
    runner.assert_matrix_value(matrix, "2", "2",  0.0120, "A[2][2]");
    runner.assert_matrix_value(matrix, "2", "3", -0.0100, "A[2][3]");
    runner.assert_matrix_value(matrix, "4", "4",  0.0020, "A[4][4]");
    
    runner.assert_rhs_value(vector, "4", 1.0, "b[4] from current source");
    
    std::remove("test9.net");
}

// TEST 10: Large Ladder Network Performance Test
void test_ladder_10000_performance(MNATestRunner& runner) {
    runner.start_test("TEST 10: Ladder Network Performance (10000 nodes)");
    
    /*
     * Tests performance on a large ladder network with 10000 nodes
     */
    
    Circuit circuit("Ladder10000");
    
    std::cout << "  Loading netlist: tests/test_netlists/ladder_10000.net" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    circuit.parse_netlist("tests/test_netlists/ladder_10000.net");
    circuit.assemble_MNA_system();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "  Assembly time: " << duration.count() << " ms" << std::endl;
    
    // Performance check: should complete in under 1 second
    runner.assert_true(duration.count() < 1000, "Assembly completes in < 1000ms");
}

// TEST 11: Tree Network Performance and Structure Test
void test_tree_d10_b3_performance(MNATestRunner& runner) {
    runner.start_test("TEST 11: Tree Network Performance (depth=10, branch=3)");
    
    /*
     * Tests performance on a tree network structure (depth 10, branching factor 3)
     */
    
    Circuit circuit("TreeD10B3");
    
    std::cout << "  Loading netlist: tests/test_netlists/tree_d10_b3.net" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    circuit.parse_netlist("tests/test_netlists/tree_d10_b3.net");
    circuit.assemble_MNA_system();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "  Assembly time: " << duration.count() << " ms" << std::endl;
    
    // Performance check
    runner.assert_true(duration.count() < 1000, "Assembly completes in < 1000ms");
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "       MNA MATRIX VALUE VERIFICATION TEST SUITE" << std::endl;
    std::cout << "           Using Helper Function Parsers" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    
    MNATestRunner runner;
    
    test_single_resistor(runner);
    test_series_resistors(runner);
    test_parallel_resistors(runner);
    test_voltage_source(runner);
    test_current_source(runner);
    test_voltage_divider(runner);
    test_three_node_network(runner);
    test_matrix_symmetry(runner);
    test_original_circuit(runner);
    test_ladder_10000_performance(runner);
    test_tree_d10_b3_performance(runner);
    
    runner.print_summary();
    
    return 0;
}