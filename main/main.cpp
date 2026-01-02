#include "Include.h"

using namespace std;

const string CWD = "main/";

int main(int argc, char* argv[]) {
    Timer timer;
    Circuit circuit;
    string netlist_file_path = CWD + "netlist.txt";
    circuit.parse_netlist(netlist_file_path);
    circuit.assemble_MNA_system();
    cout << circuit << endl;
    return 0;
}