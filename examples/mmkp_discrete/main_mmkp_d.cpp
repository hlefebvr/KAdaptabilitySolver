//
// Created by henri on 21/06/21.
//

#include <iostream>
#include <fstream>
#include "robustSolver.hpp"
#include "problemInfo_mmkp_d.h"
#include "../mmkp_continuous/mmkp_Instance.h"

int main(int argc, const char** argv) {

    const bool heuristic_mode = false;
    const unsigned int Kmax = 10;
    const bool generate_new_instance = true;

    mmkp_Instance* instance = nullptr;

    if (!generate_new_instance) {
        instance = new mmkp_Instance("instance.txt", 0);
    } else {
        instance = new mmkp_Instance();
        std::ofstream file("instance.txt");
        file << *instance;
        file.close();
    }

    KAdaptableInfo_MMKP info(*instance);
    info.build();

    KAdaptableSolver solver(info);
    std::vector<double> x;

    // solving in a loop the 1-,2-,...,K-adaptability problems
    // will automatically warm-start from the previous solve
    for (unsigned int K = 1; K <= Kmax; K++) {
        solver.solve_KAdaptability(K, heuristic_mode, x);
    }

    return 0;
}
