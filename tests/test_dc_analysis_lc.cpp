/**
 * @file test_dc_analysis_lc.cpp
 * @brief DC Analysis Test Suite for Inductors and Capacitors
 * @version 2.5.0
 * 
 * Tests DC behavior of L and C components:
 * - Inductors act as SHORT CIRCUIT (wire) at DC steady state
 * - Capacitors act as OPEN CIRCUIT (infinite impedance) at DC steady state
 * 
 * Note: Voltage sources follow the convention V<name> <+node> <-node> <value>
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <stdexcept>

#include "simulator.h"

// ============================================================================
// TEST CASE STRUCTURE
// ============================================================================

struct TestCase {
    std::string name;
    std::string description;
    std::string netlist_content;
    std::map<std::string, double> expected_voltages;
    std::map<std::string, double> expected_currents;
    double voltage_tolerance;
    double current_tolerance;
    
    TestCase(const std::string& n, 
             const std::string& desc,
             const std::string& netlist,
             double v_tol = 1e-6,
             double i_tol = 1e-9)
        : name(n), 
          description(desc), 
          netlist_content(netlist),
          voltage_tolerance(v_tol), 
          current_tolerance(i_tol) {}
};

// ============================================================================
// TEST RESULT STRUCTURE
// ============================================================================

struct TestResult {
    std::string test_name;
    bool passed;
    double execution_time_ms;
    std::vector<std::string> errors;
    std::map<std::string, double> actual_voltages;
    
    TestResult(const std::string& name) 
        : test_name(name), passed(true), execution_time_ms(0.0) {}
    
    void add_error(const std::string& error) {
        errors.push_back(error);
        passed = false;
    }
};

// ============================================================================
// TEST RUNNER CLASS
// ============================================================================

class TestRunner {
private:
    std::vector<TestCase> test_cases;
    std::vector<TestResult> test_results;
    int total_tests;
    int passed_tests;
    int failed_tests;
    double total_execution_time;
    
    bool approx_equal(double a, double b, double tolerance) const {
        return std::abs(a - b) <= tolerance;
    }
    
    std::string create_temp_netlist(const std::string& content, 
                                   const std::string& test_name) {
        std::string filename = "temp_" + test_name + ".net";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create temporary netlist file");
        }
        file << content;
        file.close();
        return filename;
    }
    
    void validate_voltages(const TestCase& test,
                          const Circuit& circuit,
                          TestResult& result) {
        const auto& nodes = circuit.get_nodes();
        
        for (const auto& [node_id, expected_voltage] : test.expected_voltages) {
            auto it = nodes.find(node_id);
            
            if (it == nodes.end()) {
                result.add_error("Node '" + node_id + "' not found in circuit");
                continue;
            }
            
            double actual_voltage = it->second->voltage;
            result.actual_voltages[node_id] = actual_voltage;
            
            if (!approx_equal(actual_voltage, expected_voltage, test.voltage_tolerance)) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(9);
                oss << "Voltage mismatch at node '" << node_id << "': "
                    << "expected " << expected_voltage << " V, "
                    << "got " << actual_voltage << " V "
                    << "(error: " << std::abs(actual_voltage - expected_voltage) << " V)";
                result.add_error(oss.str());
            }
        }
    }
    
    TestResult execute_test(const TestCase& test) {
        TestResult result(test.name);
        std::string netlist_file;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            netlist_file = create_temp_netlist(test.netlist_content, test.name);

            Node::valid = false;
            Node::node_count = 0;

            Circuit circuit(test.name);
            circuit.parse_netlist(netlist_file);
            circuit.assemble_MNA_system();

            Simulator simulator;
            simulator.run_dc_analysis(circuit);

            validate_voltages(test, circuit, result);
            
            std::remove(netlist_file.c_str());
            
        } catch (const std::exception& e) {
            result.add_error(std::string("Exception: ") + e.what());
            if (!netlist_file.empty()) {
                std::remove(netlist_file.c_str());
            }
        } catch (...) {
            result.add_error("Unknown exception occurred");
            if (!netlist_file.empty()) {
                std::remove(netlist_file.c_str());
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end_time - start_time;
        result.execution_time_ms = duration.count();
        
        return result;
    }

public:
    TestRunner()
        : total_tests(0),
          passed_tests(0),
          failed_tests(0),
          total_execution_time(0.0) {
    }
    
    void add_test_case(const TestCase& test) {
        test_cases.push_back(test);
    }
    
    bool run_all_tests() {
        std::cout << "\n========================================\n";
        std::cout << "DC ANALYSIS L/C TEST SUITE v2.5.0\n";
        std::cout << "========================================\n\n";
        
        test_results.clear();
        total_tests = test_cases.size();
        passed_tests = 0;
        failed_tests = 0;
        total_execution_time = 0.0;
        
        std::cout << "Running " << total_tests << " test(s)...\n\n";
        
        for (size_t i = 0; i < test_cases.size(); ++i) {
            const auto& test = test_cases[i];
            
            std::cout << "[" << std::setw(2) << (i + 1) << "/" 
                      << std::setw(2) << total_tests << "] ";
            std::cout << std::setw(45) << std::left << test.name;
            
            TestResult result = execute_test(test);
            test_results.push_back(result);
            total_execution_time += result.execution_time_ms;
            
            if (result.passed) {
                passed_tests++;
                std::cout << " PASSED";
            } else {
                failed_tests++;
                std::cout << " FAILED";
            }
            
            std::cout << " (" << std::fixed << std::setprecision(2) 
                      << std::setw(6) << result.execution_time_ms << " ms)\n";
            
            if (!result.passed) {
                for (const auto& error : result.errors) {
                    std::cout << "    Error: " << error << "\n";
                }
            }
        }
        
        return failed_tests == 0;
    }
    
    void print_summary() {
        std::cout << "\n========================================\n";
        std::cout << "TEST SUMMARY\n";
        std::cout << "========================================\n\n";
        
        std::cout << "Total Tests:     " << total_tests << "\n";
        std::cout << "Passed:          " << passed_tests << "\n";
        std::cout << "Failed:          " << failed_tests << "\n";
        
        double pass_rate = total_tests > 0 ? 
            (100.0 * passed_tests / total_tests) : 0.0;
        std::cout << "Pass Rate:       " << std::fixed << std::setprecision(1) 
                  << pass_rate << "%\n";
        
        std::cout << "Total Time:      " << std::fixed << std::setprecision(2)
                  << total_execution_time << " ms\n";
        
        if (total_tests > 0) {
            std::cout << "Average Time:    " << std::fixed << std::setprecision(2)
                      << (total_execution_time / total_tests) << " ms/test\n";
        }
        
        std::cout << "\n";
        
        if (failed_tests > 0) {
            std::cout << "Failed Tests:\n";
            for (const auto& result : test_results) {
                if (!result.passed) {
                    std::cout << "  - " << result.test_name << "\n";
                }
            }
            std::cout << "\n";
        }
    }
};

// ============================================================================
// CAPACITOR BASIC TESTS
// ============================================================================

void setup_capacitor_basic_tests(TestRunner& runner) {
    // Test 1: Single Capacitor - Open Circuit
    {
        TestCase test("Cap_Basic_OpenCircuit", 
                     "Capacitor blocks DC current (open circuit)",
                     "* Single Capacitor Test\n"
                     "V1 1 0 10\n"
                     "C1 1 2 0.0001\n"
                     "R1 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 0.0;  // No current through C, so no voltage across R
        runner.add_test_case(test);
    }
    
    // Test 2: Capacitor Voltage Equals Source
    {
        TestCase test("Cap_Basic_ChargedToSource",
                     "Capacitor charges to source voltage",
                     "* Capacitor Charging Test\n"
                     "V1 1 0 5\n"
                     "C1 1 0 0.00001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 5.0;  // Capacitor charged to 5V
        runner.add_test_case(test);
    }
    
    // Test 3: Series RC Circuit
    {
        TestCase test("Cap_Basic_SeriesRC",
                     "Series RC circuit at DC steady state",
                     "* Series RC\n"
                     "V1 1 0 12\n"
                     "R1 1 2 1000\n"
                     "C1 2 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 12.0;  // Capacitor charged to source voltage
        runner.add_test_case(test);
    }
    
    // Test 4: Parallel RC Circuit
    {
        TestCase test("Cap_Basic_ParallelRC",
                     "Parallel RC circuit",
                     "* Parallel RC\n"
                     "V1 1 0 10\n"
                     "R1 1 0 1000\n"
                     "C1 1 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;  // Both R and C see source voltage
        runner.add_test_case(test);
    }
    
    // Test 5: Capacitor Divider (Both Open)
    {
        TestCase test("Cap_Basic_CapacitorDivider",
                     "Series capacitors act as open circuit",
                     "* Capacitor Divider\n"
                     "V1 1 0 10\n"
                     "C1 1 2 0.0001\n"
                     "C2 2 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 0.0;  // Voltage divides equally
        runner.add_test_case(test);
    }
}

// ============================================================================
// INDUCTOR BASIC TESTS
// ============================================================================

void setup_inductor_basic_tests(TestRunner& runner) {
    // Test 6: Single Inductor - Short Circuit
    {
        TestCase test("Ind_Basic_ShortCircuit",
                     "Inductor acts as short circuit (wire) at DC",
                     "* Single Inductor Test\n"
                     "V1 1 0 10\n"
                     "L1 1 2 0.01\n"
                     "R1 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 10.0;  // Inductor is wire, same voltage
        runner.add_test_case(test);
    }
    
    // Test 7: Series RL Circuit
    {
        TestCase test("Ind_Basic_SeriesRL",
                     "Series RL circuit at DC steady state",
                     "* Series RL\n"
                     "V1 1 0 12\n"
                     "R1 1 2 100\n"
                     "L1 2 3 0.001\n"
                     "R2 3 0 100\n",
                     1e-3);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 6.0;   // Voltage divider
        test.expected_voltages["3"] = 6.0;   // Inductor has no voltage drop
        runner.add_test_case(test);
    }
    
    // Test 8: Parallel RL Circuit
    {
        TestCase test("Ind_Basic_ParallelRL",
                     "Parallel RL circuit",
                     "* Parallel RL\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "R2 2 0 1000\n"
                     "L2 2 0 0.01\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;  
        test.expected_voltages["2"] = 0.0;   // Inductor shorts node to ground
        runner.add_test_case(test);
    }
    
    // Test 9: Inductor in Series (Multiple)
    {
        TestCase test("Ind_Basic_SeriesInductors",
                     "Multiple inductors in series act as single wire",
                     "* Series Inductors\n"
                     "V1 1 0 10\n"
                     "L1 1 2 0.001\n"
                     "L2 2 3 0.005\n"
                     "R1 3 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 10.0;  // L1 is wire
        test.expected_voltages["3"] = 10.0;  // L2 is wire
        runner.add_test_case(test);
    }
    
    // Test 10: Current Source with Inductor
    {
        TestCase test("Ind_Basic_CurrentSource",
                     "Current source with inductor",
                     "* Current Source + Inductor\n"
                     "I1 0 1 0.01\n"
                     "L1 1 2 0.001\n"
                     "R1 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;  // I × R
        test.expected_voltages["2"] = 10.0;  // Inductor is wire
        runner.add_test_case(test);
    }
}

// ============================================================================
// RLC COMBINATION TESTS
// ============================================================================

void setup_rlc_combination_tests(TestRunner& runner) {
    // Test 11: Series RLC
    {
        TestCase test("RLC_Series",
                     "Series RLC circuit at DC steady state",
                     "* Series RLC\n"
                     "V1 1 0 12\n"
                     "R1 1 2 100\n"
                     "L1 2 3 0.01\n"
                     "C1 3 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 12.0;  // After R (no current)
        test.expected_voltages["3"] = 12.0;  // C blocks, L is wire
        runner.add_test_case(test);
    }
    
    // Test 12: Parallel RLC
    {
        TestCase test("RLC_Parallel",
                     "Parallel RLC circuit",
                     "* Parallel RLC\n"
                     "V1 1 0 10\n"
                     "R1 1 0 1000\n"
                     "L1 1 0 0.001\n"
                     "C1 1 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;  // All see source voltage
        runner.add_test_case(test);
    }
    
    // Test 13: RC with Load
    {
        TestCase test("RLC_RC_WithLoad",
                     "RC circuit with load resistor",
                     "* RC with Load\n"
                     "V1 1 0 12\n"
                     "R1 1 2 100\n"
                     "C1 2 3 0.0001\n"
                     "R2 3 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 12.0;  // No current through C
        test.expected_voltages["3"] = 0.0;   // No current through R2
        runner.add_test_case(test);
    }
    
    // Test 14: RL Voltage Divider
    {
        TestCase test("RLC_RL_Divider",
                     "RL voltage divider",
                     "* RL Divider\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "L1 2 3 0.01\n"
                     "R2 3 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 5.0;   // Voltage divider
        test.expected_voltages["3"] = 5.0;   // L is wire
        runner.add_test_case(test);
    }
    
    // Test 15: Complex RLC Network
    {
        TestCase test("RLC_Complex_Network",
                     "Complex RLC network",
                     "* Complex RLC\n"
                     "V1 1 0 12\n"
                     "R1 1 2 100\n"
                     "L1 2 3 0.01\n"
                     "C1 3 0 0.0001\n"
                     "R2 3 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 12.0;  // No current
        test.expected_voltages["3"] = 12.0;  // C charged to source
        runner.add_test_case(test);
    }
}

// ============================================================================
// ADVANCED LC TESTS
// ============================================================================

void setup_advanced_lc_tests(TestRunner& runner) {
    // Test 16: LC Tank (No Oscillation at DC)
    {
        TestCase test("LC_Tank_DC",
                     "LC tank circuit at DC steady state",
                     "* LC Tank\n"
                     "V1 1 0 10\n"
                     "L1 1 2 0.001\n"
                     "C1 2 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 10.0;  // L is wire, C charged
        runner.add_test_case(test);
    }
    
    // Test 17: Multiple Capacitors in Series
    {
        TestCase test("LC_Cap_Series_Multiple",
                     "Three capacitors in series",
                     "* Three Capacitors in Series\n"
                     "V1 1 0 12\n"
                     "C1 1 2 0.0001\n"
                     "C2 2 3 0.0001\n"
                     "C3 3 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 8.0;   // Voltage divides
        test.expected_voltages["3"] = 4.0;
        runner.add_test_case(test);
    }
    
    // Test 18: Multiple Inductors in Parallel
    {
        TestCase test("LC_Ind_Parallel_Multiple",
                     "Multiple inductors in parallel",
                     "* Parallel Inductors\n"
                     "V1 1 0 10\n"
                     "L1 1 2 0.001\n"
                     "L2 1 2 0.005\n"
                     "R1 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 10.0;  // All inductors are wires
        runner.add_test_case(test);
    }
    
    // Test 19: Capacitor Bypass
    {
        TestCase test("LC_Cap_Bypass",
                     "Capacitor bypass configuration",
                     "* Capacitor Bypass\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "R2 2 0 1000\n"
                     "C1 2 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 5.0;   // Voltage divider (C doesn't affect DC)
        runner.add_test_case(test);
    }
    
    // Test 20: Inductor Choke
    {
        TestCase test("LC_Ind_Choke",
                     "Inductor as DC choke",
                     "* Inductor Choke\n"
                     "V1 1 0 12\n"
                     "L1 1 2 0.1\n"
                     "R1 2 0 100\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 12.0;  // Inductor passes DC
        runner.add_test_case(test);
    }
}

// ============================================================================
// EDGE CASE TESTS
// ============================================================================

void setup_lc_edge_case_tests(TestRunner& runner) {
    // Test 21: Very Large Capacitor
    {
        TestCase test("LC_Edge_LargeCap",
                     "Very large capacitance",
                     "* Large Capacitor\n"
                     "V1 1 0 10\n"
                     "C1 1 2 1\n"
                     "R1 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 0.0;   // Still blocks DC
        runner.add_test_case(test);
    }
    
    // Test 22: Very Small Capacitor
    {
        TestCase test("LC_Edge_SmallCap",
                     "Very small capacitance",
                     "* Small Capacitor\n"
                     "V1 1 0 10\n"
                     "C1 1 2 1e-12\n"
                     "R1 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 0.0;   // Still blocks DC
        runner.add_test_case(test);
    }
    
    // Test 23: Very Large Inductor
    {
        TestCase test("LC_Edge_LargeInd",
                     "Very large inductance",
                     "* Large Inductor\n"
                     "V1 1 0 10\n"
                     "L1 1 2 100\n"
                     "R1 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 10.0;  // Still acts as wire
        runner.add_test_case(test);
    }
    
    // Test 24: Very Small Inductor
    {
        TestCase test("LC_Edge_SmallInd",
                     "Very small inductance",
                     "* Small Inductor\n"
                     "V1 1 0 10\n"
                     "L1 1 2 1e-9\n"
                     "R1 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 10.0;  // Still acts as wire
        runner.add_test_case(test);
    }
    
    // Test 25: Mixed Extreme Values
    {
        TestCase test("LC_Edge_MixedExtremes",
                     "Circuit with extreme L and C values",
                     "* Mixed Extremes\n"
                     "V1 1 0 12\n"
                     "L1 1 2 1e-9\n"
                     "C1 2 3 1e-12\n"
                     "R1 3 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 12.0;  // L is wire
        test.expected_voltages["3"] = 0.0;   // C blocks
        runner.add_test_case(test);
    }
}

// ============================================================================
// PRACTICAL CIRCUIT TESTS
// ============================================================================

void setup_practical_lc_tests(TestRunner& runner) {
    // Test 26: Power Supply Filter
    {
        TestCase test("LC_Practical_PSU_Filter",
                     "Power supply LC filter",
                     "* PSU LC Filter\n"
                     "V1 1 0 12\n"
                     "L1 1 2 0.1\n"
                     "C1 2 0 0.001\n"
                     "R1 2 0 100\n",
                     1e-3);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 12.0;  // L passes DC, C charged
        runner.add_test_case(test);
    }
    
    // Test 27: Coupling Capacitor
    {
        TestCase test("LC_Practical_Coupling",
                     "AC coupling capacitor (DC blocking)",
                     "* Coupling Capacitor\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "C1 2 3 0.00001\n"
                     "R2 3 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 10.0;  // No current through R1
        test.expected_voltages["3"] = 0.0;   // C blocks DC
        runner.add_test_case(test);
    }
    
    // Test 28: Decoupling Capacitor
    {
        TestCase test("LC_Practical_Decoupling",
                     "Decoupling capacitor configuration",
                     "* Decoupling Cap\n"
                     "V1 1 0 5\n"
                     "R1 1 2 10\n"
                     "C1 2 0 0.0001\n"
                     "R2 2 0 1000\n",
                     1e-3);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 5.0;
        test.expected_voltages["2"] = 4.9505;  // Voltage divider (R1 and R2)
        runner.add_test_case(test);
    }
    
    // Test 29: RF Choke
    {
        TestCase test("LC_Practical_RF_Choke",
                     "RF choke inductor",
                     "* RF Choke\n"
                     "V1 1 0 12\n"
                     "L1 1 2 0.001\n"
                     "R1 2 3 100\n"
                     "C1 3 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 12.0;  // L is wire at DC
        test.expected_voltages["3"] = 12.0;  // No current, C charged
        runner.add_test_case(test);
    }
    
    // Test 30: Bias Network
    {
        TestCase test("LC_Practical_Bias_Network",
                     "Transistor bias network with bypass cap",
                     "* Bias Network\n"
                     "V1 1 0 12\n"
                     "R1 1 2 10000\n"
                     "R2 2 0 10000\n"
                     "C1 2 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 6.0;   // Voltage divider (C doesn't affect DC)
        runner.add_test_case(test);
    }
}

// ============================================================================
// VALIDATION TESTS
// ============================================================================

void setup_lc_validation_tests(TestRunner& runner) {
    // Test 31: Energy Storage Verification
    {
        TestCase test("LC_Validation_Energy",
                     "Verify capacitor energy storage at DC",
                     "* Energy Storage\n"
                     "V1 1 0 10\n"
                     "R1 1 2 100\n"
                     "C1 2 0 0.0001\n"
                     "C2 2 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 10.0;  // Both caps charged to 10V
        runner.add_test_case(test);
    }
    
    // Test 32: Inductor Current Continuity
    {
        TestCase test("LC_Validation_Current_Continuity",
                     "Verify inductor maintains current",
                     "* Current Continuity\n"
                     "V1 1 0 12\n"
                     "L1 1 2 0.01\n"
                     "L2 2 3 0.01\n"
                     "R1 3 0 100\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 12.0;  // Both inductors are wires
        test.expected_voltages["3"] = 12.0;
        runner.add_test_case(test);
    }
    
    // Test 33: Thevenin Equivalent with LC
    {
        TestCase test("LC_Validation_Thevenin",
                     "Thevenin equivalent with LC components",
                     "* Thevenin with LC\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "L1 2 3 0.01\n"
                     "C1 3 0 0.0001\n"
                     "R2 3 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 10.0;  // No current
        test.expected_voltages["3"] = 0.0;   // C blocks
        runner.add_test_case(test);
    }
    
    // Test 34: Norton Equivalent with LC
    {
        TestCase test("LC_Validation_Norton",
                     "Norton equivalent with LC components",
                     "* Norton with LC\n"
                     "I1 0 1 0.01\n"
                     "L1 1 2 0.01\n"
                     "R1 2 0 1000\n"
                     "C1 2 0 0.0001\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;  // I × R
        test.expected_voltages["2"] = 10.0;  // L is wire
        runner.add_test_case(test);
    }
    
    // Test 35: Superposition with LC
    {
        TestCase test("LC_Validation_Superposition",
                     "Superposition principle with LC",
                     "* Superposition LC\n"
                     "V1 1 0 10\n"
                     "V2 3 0 5\n"
                     "R1 1 2 1000\n"
                     "L1 2 3 0.01\n"
                     "C1 2 0 0.0001\n",
                     1e-3);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 5.0;   // Between sources
        test.expected_voltages["3"] = 5.0;
        runner.add_test_case(test);
    }
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    TestRunner runner;
    
    std::cout << "\nDC Behavior:\n";
    std::cout << "  • Capacitors: OPEN CIRCUIT (block DC current)\n";
    std::cout << "  • Inductors:  SHORT CIRCUIT (act as wire)\n\n";
    
    // Add all test suites
    setup_capacitor_basic_tests(runner);
    setup_inductor_basic_tests(runner);
    setup_rlc_combination_tests(runner);
    setup_advanced_lc_tests(runner);
    setup_lc_edge_case_tests(runner);
    setup_practical_lc_tests(runner);
    setup_lc_validation_tests(runner);
    
    bool all_passed = runner.run_all_tests();
    runner.print_summary();
    
    return all_passed ? 0 : 1;
}