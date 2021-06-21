//
// Created by henri on 21/06/21.
//

#include "problemInfo_mmkp.h"
#include "mmkp_Instance.h"

KAdaptableInfo_MMKP::KAdaptableInfo_MMKP(const mmkp_Instance &t_instance)
        : m_instance(&t_instance) {

}

void KAdaptableInfo_MMKP::build() {
    hasInteger = true;
    objectiveUnc = false;
    existsFirstStage = false;
    numFirstStage = m_instance->n_items() * m_instance->n_knapsacks() + 1;
    numSecondStage = m_instance->n_items() * m_instance->n_knapsacks();
    numPolicies = 1;
    solfilename = "result.txt";

    makeUncSet();
    makeVars();
    makeConsX();
    makeConsY(0);

    assert(isConsistentWithDesign());
}

void KAdaptableInfo_MMKP::makeUncSet() {
    U.clear();

    const unsigned int I = m_instance->n_items();

    // define uncertain risk factors
    for (unsigned int i = 0 ; i < I ; ++i) {
        U.addParam(0, 0, 1);
    }

    // define gamma constraint
    std::vector<std::pair<int, double> > constraint;
    for (int i = 1; i <= I; ++i) { constraint.emplace_back(i, 1); }
    U.addFacet(constraint, 'L', m_instance->gamma());

}

void KAdaptableInfo_MMKP::makeVars() {
    X.clear();
    Y.clear();

    // objective variable (considered to be 1st-stage)
    X.addVarType("O", 'C', -CPX_INFBOUND, +CPX_INFBOUND, 1);
    X.setVarObjCoeff(1.0, "O", 0);

    // x(i,k) -> item i is to be packed in knapsack k
    X.addVarType("x", 'B', 0, 1, m_instance->n_items(), m_instance->n_knapsacks());

    // y(i,k) -> item i is selected
    Y.addVarType("y", 'B', 0, 1, m_instance->n_items(), m_instance->n_knapsacks());
}

void KAdaptableInfo_MMKP::makeConsX() {

    const unsigned int I = m_instance->n_items();
    const unsigned int K = m_instance->n_knapsacks();

    // B_X
    B_X.clear();
    // LB
    for (unsigned int i = 0 ; i < I ; ++i) {
        for (unsigned int k = 0 ; k < K ; ++k) {
            ConstraintExpression expr;
            expr.rowname("UB_x(" + std::to_string(i) + "," + std::to_string(k) + ")");
            expr.addTermX(getVarIndex_1("x", i, k), 1);
            expr.sign('G');
            expr.RHS(0);
            B_X.emplace_back(expr);
        }
    }
    // UB
    for (unsigned int i = 0 ; i < I ; ++i) {
        for (unsigned int k = 0 ; k < K ; ++k) {
            ConstraintExpression expr;
            expr.rowname("UB_x(" + std::to_string(i) + "," + std::to_string(k) + ")");
            expr.addTermX(getVarIndex_1("x", i, k), 1);
            expr.sign('L');
            expr.RHS(1);
            B_X.emplace_back(expr);
        }
    }

    // C_X
    C_X.clear();
    // Every item must be assigned to a knapsack
    for (unsigned int i = 0 ; i < I ; ++i) {
        ConstraintExpression expr;
        expr.rowname("AssignKnapsack(" + std::to_string(i) + ")");
        for (unsigned int k = 0 ; k < K ; ++k) {
            expr.addTermX(getVarIndex_1("x", i, k), 1);
        }
        expr.sign('E');
        expr.RHS(1);
        B_X.emplace_back(expr);
    }

    // C_XQ
    C_XQ.clear();
}

void KAdaptableInfo_MMKP::makeConsY(unsigned int L) {

    const unsigned int I = m_instance->n_items();
    const unsigned int K = m_instance->n_knapsacks();

    if (L == 0) {
        B_Y.clear();
        C_XY.clear();
        C_XYQ.clear();
    }

    // B_Y
    for (unsigned int l = B_Y.size() ; l <= L ; ++l) {

        if (B_Y.size() < l + 1) { B_Y.resize(l+1); }

        B_Y[l].clear();

        // LB
        for (unsigned int i = 0 ; i < I ; ++i) {
            for (unsigned int k = 0 ; k < K ; ++k) {
                ConstraintExpression expr;
                expr.addTermX(getVarIndex_2(l, "y", i, k), 1);
                expr.sign('G');
                expr.RHS(0);
                B_Y[l].emplace_back(expr);
            }
        }

        // UB
        for (unsigned int i = 0 ; i < I ; ++i) {
            for (unsigned int k = 0 ; k < K ; ++k) {
                ConstraintExpression expr;
                expr.addTermX(getVarIndex_2(l, "y", i, k), 1);
                expr.sign('L');
                expr.RHS(1);
                B_Y[l].emplace_back(expr);
            }
        }

    }

    // C_XY
    for (unsigned int l = C_XY.size() ; l <= L ; ++l) {

        if (C_XY.size() < l + 1) { C_XY.resize(l + 1); }

        C_XY[l].clear();

        // non-anticipative
        for (unsigned int i = 0 ; i < I ; ++i) {
            for (unsigned int k = 0 ; k < K ; ++k) {
                ConstraintExpression expr;
                expr.addTermX(getVarIndex_1("x", i, k), -1);
                expr.addTermX(getVarIndex_2(l, "y", i, k), 1);
                expr.RHS(0);
                expr.sign('L');
                C_XY[l].emplace_back(expr);
            }
        }

        // objective
        ConstraintExpression expr;
        for (unsigned int i = 0 ; i < I ; ++i) {
            for (unsigned int k = 0; k < K; ++k) {
                expr.addTermX(getVarIndex_2(l, "y", i, k), m_instance->profit(i));
            }
        }
        expr.addTermX(getVarIndex_1("O", 0), 1);
        expr.sign('G');
        expr.RHS(0);
        C_XY[l].emplace_back(expr);

    }

    // C_XYQ
    for (unsigned int l = C_XYQ.size() ; l <= L ; ++l) {

        if (C_XYQ.size() < l + 1) { C_XYQ.resize(l + 1); }

        C_XYQ[l].clear();

        // knapsacks constraints
        for (unsigned int k = 0 ; k < K ; ++k) {
            ConstraintExpression expr;
            for (unsigned int i = 0 ; i < I ; ++i) {
                expr.addTermX(getVarIndex_2(l, "y", i, k), m_instance->low_weight(i));
                expr.addTermProduct(getVarIndex_2(l, "y", i, k), U.getParamIndex('q', i+1), m_instance->high_weight(i));
            }
            expr.sign('L');
            expr.RHS(m_instance->capacity());
            C_XYQ[l].emplace_back(expr);
        }

    }

}
