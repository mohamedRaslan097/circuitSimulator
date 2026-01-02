#include "ui.h"

UI::UI() : input_file(""), output_file("output.log"), verbose(false), pause(false), program_name("circuit_simulator") {}

bool UI::parse_arguments(int argc, char* argv[]) {
    program_name = argv[0];
    
    for(int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if(arg == "-i" && i + 1 < argc) {
            input_file = argv[++i];
        } else if(arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        } else if(arg == "-h") {
            print_usage();
            return false;
        } else if(arg == "-v") {
            verbose = true;
        } else if(arg == "-p") {
            pause = true;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            print_usage();
            return false;
        }
    }
    
    // Check if input file was provided
    if(input_file.empty()) {
        std::cerr << "Error: Input file is required. Use -i <filename>" << std::endl;
        print_usage();
        return false;
    }
    
    return true;
}

void UI::print_usage() const {
    std::cout << "Usage: " << program_name << " -i input_file [-o output.log] [-v]" << std::endl;
    std::cout << "  -i <file>   Input netlist file (required)" << std::endl;
    std::cout << "  -o <file>   Output results file (default: output.log)" << std::endl;
    std::cout << "  -v          Verbose mode" << std::endl;
    std::cout << "  -h          Show help" << std::endl;
}

void UI::print_banner(std::ostream& os) const {
    os << "╔════════════════════════════════════╗\n"
       << "║   Circuit Simulator v1.0.0         ║\n"
       << "║     MNA System Assembly            ║\n"
       << "╚════════════════════════════════════╝\n\n";
}

void UI::output_results(const std::string& circuit_output) {
    // Write to file
    std::ofstream out(output_file);
    if(!out) {
        std::cerr << "Error: Could not open output file: " << output_file << std::endl;
        return;
    }

    print_banner(out);
    out << circuit_output;
    out.close();
    
    if(verbose) {
        std::cout << circuit_output;
    }
    
    std::cout << "Circuit analysis complete. Results written to: " << output_file << std::endl;
}

UI::~UI() {
    if(pause) {
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }
}