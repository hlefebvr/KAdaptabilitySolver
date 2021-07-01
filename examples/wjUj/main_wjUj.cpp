//
// Created by henri on 01/07/21.
//

#include <iostream>
#include <fstream>
#include "robustSolver.hpp"
#include "problemInfo_wjUj.h"
#include "wjUj_Instance.h"

int main(int argc, const char** argv) {

    const bool heuristic_mode = false;
    const unsigned int K = 2;
    const unsigned int gamma = 2;

    wjUj_Instance source_instance("/home/henri/CLionProjects/KAdaptabilitySolver/examples/wjUj/example.txt", gamma);
    wjUj_Instance reversed = source_instance.reversed();

    KAdaptableInfo_wjUj info(reversed);
    info.build();

    KAdaptableSolver solver(info);
    std::vector<double> x;

    solver.solve_KAdaptability(K, heuristic_mode, x);

    return 0;
}
