#include "ac_analyzer.h"

Ac_analyzer::Ac_analyzer(const std::string& output_file) : output_file(output_file) {}

void Ac_analyzer::initialize(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                             const std::map<int, std::string>& extra_vars,
                             const std::vector<double>& initial_solution) {

    // Convert real-valued MNA matrix and vector to complex
    this->mna_matrix.clear();
    this->mna_vector.clear();
    this->solution.clear();
    this->solution.resize(initial_solution.size(), std::complex<double>(0.0, 0.0));

    std::ofstream out(output_file, std::ios::trunc);
    if (!out.is_open())
        throw std::runtime_error("Failed to open AC analysis output file: " + output_file);
    log_header();
    out.close();

    for (size_t row = 1; row < initial_solution.size(); row++)
        this->solution[row] = std::complex<double>(initial_solution[row], 0.0);

    // log initial solution at DC (0 Hz)
    log_ac_inst_solution(0.0, std::chrono::microseconds(0), 0);

    for(auto [id,var]: extra_vars)
        if(toupper(var[1]) == Inductor::default_id[0])
            this->solution[id] = std::complex<double>(nan(""), nan(""));

    for (const auto& [row, col_map] : mna_matrix) {
        // Skip extra variable rows
        if (extra_vars.find(row) != extra_vars.end())
            continue;
        
        for (const auto& [col, value] : col_map) {
            // Skip extra variable columns
            if (extra_vars.find(col) != extra_vars.end())
                continue;
            
            this->mna_matrix[row][col] = std::complex<double>(value, 0.0);
        }
    }
}

void Ac_analyzer::assemble_ac_mna_system(const std::unordered_map<std::string, Component*>& ac_components, double frequency) {
    // Assemble AC contributions from components
    for (const auto& [id, component] : ac_components) {
        Component_contribution<std::complex<double>> contrib = static_cast<Ac_component*>(component)->get_ac_contribution(frequency);
        
        for (const auto& mc : contrib.matrixStamps)
            mna_matrix[mc.row][mc.col] += mc.value;
        
        for (const auto& vc : contrib.vectorStamps)
            mna_vector[vc.row] += vc.value;
    }
}

void Ac_analyzer::log_header() const {
    std::ofstream out(output_file, std::ios::app);
    // Frequency(Hz), Re(x[0]), Im(x[0]), Re(x[1]), Im(x[1]), Re(x[2]), Im(x[2]), Re(x[3]), Im(x[3]), Converge_Iters, Duration_us
    // 0, (0,0), (12,0), (-0.004,0), (8,0), 0, 0
    // 1, (0,0), (4.0117e-06,0), (-0.004,0), (3.48423e-06,0), 120, 93
    // 2, (0,0), (2.16316e-06,0), (-0.004,0), (1.87874e-06,0), 5, 9
    // ...
    if (!out.is_open())
        throw std::runtime_error("Failed to open AC analysis output file: " + output_file);
    
    out << "Frequency(Hz), ";
    for (size_t i = 0; i < solution.size(); i++) {
        out << "R(x[" << i << "]), I(x[" << i << "]), ";
    }
    out << "Converge_Iters, Duration_us" << std::endl;
    out.close();
}

void Ac_analyzer::log_ac_inst_solution(double frequency, std::chrono::microseconds duration, int converge_iters) {
    std::ofstream out(output_file, std::ios::app);
    if (!out.is_open())
        throw std::runtime_error("Failed to open AC analysis output file: " + output_file);
    out << frequency<< ", ";
    for(auto& sol : solution) {
        out << sol.real() <<  ", " << sol.imag() << ", ";
    }
    out << converge_iters << ", " << duration.count() << std::endl;
    out.close();
}

void Ac_analyzer::print(std::ostream& os) const {
    os << "AC Analyzer Status:" << std::endl;
    os << std::string(40, '-') << std::endl;
    os << "  Output File: " << output_file << std::endl;
}