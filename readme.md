# Circuit Simulator

A high-performance circuit simulation tool.

## 📦 Current Version

**Release: v2.0 - Phase 2 Complete**  
*DC Solver*

**Release: v1.0 - Phase 1 Complete**  
*MNA System Assembly*

---

## 📁 Project Structure

```
circuitSimulator/
├── include/              # Header files
├── src/                  # Implementation files
├── main/                 # Main application
├── tests/                # Test suites
└── build/                # Compiled executables
```

---

## 🚀 Quick Start

### Prerequisites
- C++17 compatible compiler (g++ recommended)
- Windows with MSYS2/MinGW (or equivalent on Linux/Mac)

### Building the Project

**Compile the main program:**
```bash
# Build main program
g++ -std=c++17 -Wall -g -I./include src/*.cpp main/main.cpp -o build/debug/main.exe

# Build tests
g++ -std=c++17 -Wall -g -I./include src/*.cpp tests/test_components.cpp -o build/debug/test_components.exe
g++ -std=c++17 -Wall -g -I./include src/*.cpp tests/test_netlist_parsing.cpp -o build/debug/test_netlist_parsing.exe
g++ -std=c++17 -Wall -g -I./include src/*.cpp tests/test_mna_assembly.cpp -o build/debug/test_mna_assembly.exe
g++ -std=c++17 -Wall -g -I./include src/*.cpp tests/test_dc_analysis.cpp -o build/debug/test_dc_analysis.exe
```

### Running the Simulator

```bash
# Basic usage
./build/debug/main.exe -i netlist.txt

# With output file
./build/debug/main.exe -i circuit.net -o results.txt

# Verbose mode
./build/debug/main.exe -i circuit.net -v

# Show help
./build/debug/main.exe -h
```

#### Command-Line Options
- `-i <file>` - Input netlist file (required)
- `-o <file>` - Output results file (default: output.log)
- `-v` - Verbose mode (display results to console)
- `-h` - Show help message

### Running Tests

**Run tests:**
```bash
./build/debug/test_components.exe
./build/debug/test_netlist_parsing.exe
./build/debug/test_mna_assembly.exe
./build/debug/test_dc_analysis.exe
```

### Netlist Format
The simulator uses a SPICE-like netlist format:
```
* Comment line
V1 node- node+ value
R1 node1 node2 resistance
I1 node- node+ current
```

Example (Wheatstone Bridge):
```
* Wheatstone Bridge
V1 1 0 10.0
R1 1 2 1000
R2 2 0 1000
R3 1 3 1000
R4 3 0 1000
R5 2 3 100
```

## ✨ Implemented Features

### V1.0: MNA Assembly ✅ COMPLETE
- ✅ Project structure and build system
- ✅ Core component classes
- ✅ Netlist parsing
- ✅ MNA system assembly

### V2.0 : DC Analysis ✅ COMPLETE
- ✅ Modified Gauss-Seidel Method solver (PIONEERED)
- ✅ DC analysis validation
- ✅ Solution verification

### Core Components
- ✅ **Resistors** - Linear resistive elements
- ✅ **Voltage Sources** - Independent DC voltage sources
- ✅ **Current Sources** - Independent DC current sources
```
╔══════════════════════════════════════════════════════╗
║           COMPONENTS QUICK REFERENCE                 ║
╠══════════════════════════════════════════════════════╣
║  RESISTOR                                            ║
║  ────────                                            ║
║  Syntax:  R<name> <n+> <n-> <value>                  ║
║  Example: R1 1 0 1000                                ║
║  Units:     (ohms)                                   ║
║  Range:   R > 0                                      ║
║                                                      ║
║  VOLTAGE SOURCE                                      ║
║  ──────────────                                      ║
║  Syntax:  V<name> <n+> <n-> <DC_value>               ║
║  Example: VCC 1 0 5                                  ║
║  Units:   V (volts)                                  ║
║  Range:   Any real number                            ║
║                                                      ║
║  CURRENT SOURCE                                      ║
║  ──────────────                                      ║
║  Syntax:  I<name> <n+> <n-> <DC_value>               ║
║  Example: I1 0 1 1                                   ║
║  Units:   A (amperes)                                ║
║  Range:   Any real number                            ║
╚══════════════════════════════════════════════════════╝
```

### Netlist Parsing
- ✅ **SPICE-like Format** - Industry-standard syntax

### Circuit Analysis
- ✅ **Modified Nodal Analysis (MNA)** - Efficient matrix assembly
- ✅ **DC Analysis Solver (OP)** - DC Operating Point Solver

### User Interface
- ✅ **Command-Line Interface** - Flexible argument parsing

---

## 📋 Phase 1: Foundation & Core Architecture ✅
### Objectives:

- Set up project structure with proper build system
- Design core data structures:
  - Component base class (resistor, voltage source, current source)
  - Node class (connection points)
  - Circuit/Netlist class
- Implement basic I/O:
  - Parse simple netlist format (SPICE-like or custom)
  - Output circuit topology

### Deliverables:

- ✅ Project compiles
- ✅ Can read a simple circuit file
- ✅ Circuit Topology represented
- ✅ Unit tests for basic components

## 🔧 Phase 2: DC Analysis Engine ✅

### Objectives:

- Implement Modified Nodal Analysis (MNA):
  - Build conductance matrix (G matrix)
  - Build source vector (I vector)
  - Solve linear system: Gv = I
- Add linear solver:
  - Start with Gauss-Seidel
  - Later: LU decomposition 🔧
- Support basic components:
  - Resistors
  - Independent voltage/current sources
  - Ground node handling

### Deliverables:

- ✅ DC operating point calculation
- ✅ Node voltage outputs
- ✅ Branch current calculations
- ✅ Validation against known circuits (voltage divider, Wheatstone bridge)


## 📊 Phase 3: AC Analysis

### Objectives:


- Component base class (capacitor, inductor, AC source)
- Frequency domain analysis:
  - Small-signal linearization
  - Complex arithmetic for impedances
  - Frequency sweep
- Calculate frequency response:
  - Magnitude and phase
  - Bode plots data generation

### Deliverables:

- ✅ AC sweep from fmin to fmax
- ✅ Filter response (low-pass, high-pass, band-pass)
- ✅ Gain and phase output
- ✅ Input/output impedance calculation

## ⚡ Phase 4: Transient Analysis

### Objectives:

- Implement time-domain simulation:
  - Numerical integration (Backward Euler or Trapezoidal)
  - Timestep control
- Add dynamic components:
  - Capacitors (companion model)
  - Inductors (companion model)
- Time-varying sources:
  - Pulse sources
  - Sinusoidal sources
  - PWL (Piecewise Linear) sources

### Deliverables:

- ✅ RC/RL/RLC circuit transient response
- ✅ Output waveform data (CSV format)
- ✅ Configurable simulation time and timestep
- ✅ Energy conservation verification

## 🔬 Phase 5: Nonlinear Components

### Objectives:

- Implement Newton-Raphson solver:
  - Jacobian matrix calculation
  - Iterative convergence
- Add nonlinear devices:
  - Diode (exponential I-V characteristic)
  - BJT transistor (Ebers-Moll model)
  - MOSFET (simple Level 1 model)
- Convergence handling:
  - Damping strategies
  - Source stepping
  - Continuation methods

### Deliverables:

- ✅ Diode rectifier circuit simulation
- ✅ BJT amplifier bias point
- ✅ MOSFET inverter analysis
- ✅ Convergence diagnostics

## 🚀 Phase 6: Optimization & Advanced Features
Objectives:

Performance optimization:
Sparse matrix techniques
Parallel processing (OpenMP)
Memory profiling


Advanced analyses:
Fourier analysis (FFT)
Noise analysis
Sensitivity analysis


Model library:
Op-amp models
Transformer models
Transmission lines



### Deliverables:

- ✅ 10x performance improvement
- ✅ Component library with 20+ models
- ✅ Benchmark suite
- ✅ Documentation and examples
