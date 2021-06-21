/***************************************************************************************/
/*                                                                                     */
/*  Copyright 2018 by Anirudh Subramanyam, Chrysanthos Gounaris and Wolfram Wiesemann  */
/*                                                                                     */
/*  Licensed under the FreeBSD License (the "License").                                */
/*  You may not use this file except in compliance with the License.                   */
/*  You may obtain a copy of the License at                                            */
/*                                                                                     */
/*  https://www.freebsd.org/copyright/freebsd-license.html                             */
/*                                                                                     */
/***************************************************************************************/


#include "problemInfo_knp.hpp"
#include "robustSolver.hpp"

int main (int, char*[]) {

	const bool heuristic_mode = false;
	const unsigned int Kmax = 4;
	KAdaptableInfo *pInfo;

	try {
		// Generate the instance data
		KNP data;
		KAdaptableInfo_KNP knpInfo;
		gen_KNP(data, 10, 0, false); // set to 'true' to allow option of loans
		knpInfo.setInstance(data);

		// Clone the KAdaptableInfo object
		pInfo = knpInfo.clone();

		// CALL THE SOLVER
		KAdaptableSolver S(*pInfo);
		std::vector<double> x;
		// solving in a loop the 1-,2-,...,K-adaptability problems
		// will automatically warm-start from the previous solve
		for (unsigned int K = 1; K <= Kmax; K++) {
			S.solve_KAdaptability(K, heuristic_mode, x);
		}

		// Uncomment to also solve without warm-start -- only in exact mode
		// S.solve_KAdaptability(K, false, x);

		delete pInfo;
		pInfo = NULL;
	}
	catch (const int& e) {
		std::cerr << "Program ABORTED: Error number " << e << "\n";
	}


	return 0;
}

