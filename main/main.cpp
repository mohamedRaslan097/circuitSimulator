#include "Include.h"
#include "ui.h"

using namespace std;

int main(int argc, char* argv[]) {
    Timer timer;
    
    // Parse command-line arguments
    UI ui;
    if(!ui.parse_arguments(argc, argv)) {
        return 1;
    }
    
    // Parse and assemble circuit
    Circuit circuit;
    circuit.parse_netlist(ui.get_input_file());
    circuit.assemble_MNA_system();
    
    // Generate output
    stringstream ss;
    ss << circuit << endl;
    string circuit_output = ss.str();
    
    // Output results
    ui.output_results(circuit_output);
    
    return 0;
}