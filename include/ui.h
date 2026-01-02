#ifndef UI_H
#define UI_H

#include <iostream>
#include <fstream>
#include <string>

class UI {
private:
    std::string input_file;
    std::string output_file;
    bool verbose;
    bool pause;
    std::string program_name;
    
    void print_usage() const;
    void print_banner(std::ostream& os = std::cout) const;
    
public:
    UI();
    bool parse_arguments(int argc, char* argv[]);
    void output_results(const std::string& circuit_output);
    const std::string& get_input_file() const { return input_file; }
    ~UI();
};

#endif // UI_H
