/**
 * @file test_dc_analysis.cpp
 * @brief DC Analysis Test Suite
 * @version 2.0.0
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
        std::cout << "DC ANALYSIS TEST SUITE v2.0.0\n";
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
            std::cout << std::setw(40) << std::left << test.name;
            
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
// TEST CASE SETUP FUNCTIONS
// ============================================================================

void setup_basic_tests(TestRunner& runner) {
    // Test 1: Equal Resistor Voltage Divider
    {
        TestCase test("VoltageDevider_Equal", 
                     "Equal resistor voltage divider",
                     "* Voltage Divider Test\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "R2 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 5.0;
        runner.add_test_case(test);
    }
    
    // Test 2: Unequal Resistor Voltage Divider
    {
        TestCase test("VoltageDevider_Unequal",
                     "Unequal resistor voltage divider",
                     "* Unequal Voltage Divider\n"
                     "V1 1 0 12\n"
                     "R1 1 2 2000\n"
                     "R2 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 12.0;
        test.expected_voltages["2"] = 4.0;
        runner.add_test_case(test);
    }
    
    // Test 3: Current Source with Single Resistor
    {
        TestCase test("CurrentSource_Single",
                     "Single current source with resistor",
                     "* Current Source Test\n"
                     "I1 0 1 0.001\n"
                     "R1 1 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 1.0;
        runner.add_test_case(test);
    }
    
    // Test 4: Single Resistor Circuit
    {
        TestCase test("SingleResistor",
                     "Simplest circuit",
                     "* Single Resistor\n"
                     "V1 1 0 5\n"
                     "R1 1 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 5.0;
        runner.add_test_case(test);
    }
    
    // Test 5: Current Divider
    {
        TestCase test("CurrentDivider",
                     "Current divider with parallel resistors",
                     "* Current Divider\n"
                     "I1 0 1 0.001\n"
                     "R1 1 0 1000\n"
                     "R2 1 0 2000\n",
                     1e-4);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 0.6667;
        runner.add_test_case(test);
    }
}

void setup_network_tests(TestRunner& runner) {
    // Test 6: Balanced Wheatstone Bridge
    {
        TestCase test("Wheatstone_Balanced",
                     "Balanced Wheatstone bridge",
                     "* Balanced Wheatstone Bridge\n"
                     "V1 1 0 5\n"
                     "R1 1 2 1000\n"
                     "R2 1 3 1000\n"
                     "R3 2 0 1000\n"
                     "R4 3 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 5.0;
        test.expected_voltages["2"] = 2.5;
        test.expected_voltages["3"] = 2.5;
        runner.add_test_case(test);
    }
    
    // Test 7: Unbalanced Wheatstone Bridge
    {
        TestCase test("Wheatstone_Unbalanced",
                     "Unbalanced Wheatstone bridge",
                     "* Unbalanced Wheatstone Bridge\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "R2 1 3 2000\n"
                     "R3 2 0 1000\n"
                     "R4 3 0 1000\n",
                     1e-4);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 5.0;
        test.expected_voltages["3"] = 3.3333;
        runner.add_test_case(test);
    }
    
    // Test 8: Pi Network
    {
        TestCase test("PiNetwork",
                     "Pi resistor network",
                     "* Pi Network\n"
                     "V1 1 0 10\n"
                     "R1 1 0 1000\n"
                     "R2 1 2 1000\n"
                     "R3 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 5.0;
        runner.add_test_case(test);
    }
    
    // Test 9: T Network
    {
        TestCase test("TNetwork",
                     "T resistor network",
                     "* T Network\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "R2 2 3 1000\n"
                     "R3 2 0 1000\n",
                     1e-1);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 5.0;
        test.expected_voltages["3"] = 5.0;
        runner.add_test_case(test);
    }
    
    // Test 10: Ladder Network
    {
        TestCase test("LadderNetwork",
                     "Three-stage resistor ladder",
                     "* Three-Stage Ladder\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "R2 2 3 1000\n"
                     "R3 3 0 1000\n",
                     1e-2);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 6.6667;
        test.expected_voltages["3"] = 3.3333;
        runner.add_test_case(test);
    }
}

void setup_complex_tests(TestRunner& runner) {
    // Test 11: Multiple Voltage Sources in Series
    {
        TestCase test("MultipleSources_Series",
                     "Multiple voltage sources in series",
                     "* Multiple Voltage Sources in Series\n"
                     "V1 1 0 5\n"
                     "V2 2 1 3\n"
                     "R1 2 0 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 5.0;
        test.expected_voltages["2"] = 8.0;
        runner.add_test_case(test);
    }
    
    // Test 12: Opposing Voltage Sources
    {
        TestCase test("MultipleSources_Opposing",
                     "Opposing voltage sources",
                     "* Opposing Voltage Sources\n"
                     "V1 1 0 10\n"
                     "V2 2 0 6\n"
                     "R1 1 2 1000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 6.0;
        runner.add_test_case(test);
    }
    
    // Test 13: Series-Parallel Network
    {
        TestCase test("SeriesParallel",
                     "Series-parallel resistor network",
                     "* Series-Parallel Network\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "R2 2 0 1000\n"
                     "R3 2 0 1000\n",
                     1e-4);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 3.3333;
        runner.add_test_case(test);
    }
    
    // Test 14: Mixed Sources
    {
        TestCase test("MixedSources",
                     "Circuit with voltage and current sources",
                     "* Mixed Sources\n"
                     "V1 1 0 10\n"
                     "I1 0 2 0.005\n"
                     "R1 1 2 1000\n"
                     "R2 2 0 1000\n",
                     1e-3);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        runner.add_test_case(test);
    }
}

void setup_edge_case_tests(TestRunner& runner) {
    // Test 15: Very Small Resistance
    {
        TestCase test("EdgeCase_SmallR",
                     "Circuit with very small resistance",
                     "* Very Small Resistance\n"
                     "V1 1 0 10\n"
                     "R1 1 2 0.001\n"
                     "R2 2 0 1000\n",
                     1e-4);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 9.99999;
        runner.add_test_case(test);
    }
    
    // Test 16: Very Large Resistance
    {
        TestCase test("EdgeCase_LargeR",
                     "Circuit with very large resistance",
                     "* Very Large Resistance\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1e9\n"
                     "R2 2 0 1000\n",
                     1e-4);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 0.00001;
        runner.add_test_case(test);
    }
    
    // Test 17: Wide Range of Resistances
    {
        TestCase test("EdgeCase_WideRange",
                     "Circuit with wide range of resistances",
                     "* Wide Range of Resistances\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1\n"
                     "R2 2 3 1000\n"
                     "R3 3 0 1e6\n",
                     1e-3);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        runner.add_test_case(test);
    }
}

void setup_validation_tests(TestRunner& runner) {
    // Test 18: KCL Verification
    {
        TestCase test("Validation_KCL",
                     "Kirchhoff's Current Law verification",
                     "* KCL Test\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "R2 2 0 2000\n"
                     "R3 2 0 3000\n");
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        runner.add_test_case(test);
    }
    
    // Test 19: Symmetry Test
    {
        TestCase test("Validation_Symmetry",
                     "Symmetric circuit test",
                     "* Symmetry Test\n"
                     "V1 1 0 10\n"
                     "R1 1 2 1000\n"
                     "R2 1 3 1000\n"
                     "R3 2 4 1000\n"
                     "R4 3 4 1000\n"
                     "R5 4 0 1000\n",
                     1e-1);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 7.5;
        test.expected_voltages["3"] = 7.5;
        test.expected_voltages["4"] = 5.0;
        runner.add_test_case(test);
    }
    
    // Test 20: Superposition Principle
    {
        TestCase test("Validation_Superposition",
                     "Superposition principle test",
                     "* Superposition Test\n"
                     "V1 1 0 10\n"
                     "V2 2 0 5\n"
                     "R1 1 3 1000\n"
                     "R2 2 3 1000\n"
                     "R3 3 0 1000\n",
                     1e-3);
        test.expected_voltages["0"] = 0.0;
        test.expected_voltages["1"] = 10.0;
        test.expected_voltages["2"] = 5.0;
        runner.add_test_case(test);
    }
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    TestRunner runner;
    
    setup_basic_tests(runner);
    setup_network_tests(runner);
    setup_complex_tests(runner);
    setup_edge_case_tests(runner);
    setup_validation_tests(runner);
    
    bool all_passed = runner.run_all_tests();
    runner.print_summary();
    
    return all_passed ? 0 : 1;
}