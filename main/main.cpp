#include "Include.h"
#include "ui.h"

using namespace std;

int main(int argc, char* argv[]) {
    
    // Parse command-line arguments
    UI ui;
    if(!ui.parse_arguments(argc, argv)) {
        return 1;
    }
    
    // Start timer
    Timer timer;

    // Parse and assemble circuit
    Circuit circuit;
    circuit.parse_netlist(ui.get_input_file());
    circuit.assemble_MNA_system();
    
    // Run simulation
    Simulator simulator(ui.get_ac_output_file());
    simulator.run_dc_analysis(circuit);
    simulator.run_ac_analysis(circuit, 1, 100000, 10, true); // 1Hz to 100kHz, log scale
    
    // Generate output
    stringstream ss;
    ss << circuit << endl;
    ss << simulator << endl;
    string circuit_output = ss.str();
    
    // Output results
    ui.output_results(circuit_output);
    
    return 0;
}