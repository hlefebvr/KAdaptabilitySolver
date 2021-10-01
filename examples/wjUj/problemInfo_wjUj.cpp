//
// Created by henri on 01/07/21.
//

#include "problemInfo_wjUj.h"

KAdaptableInfo_wjUj::KAdaptableInfo_wjUj(const wjUj_Instance &t_instance) : m_instance(&t_instance) {

}

KAdaptableInfo *KAdaptableInfo_wjUj::create() const {
    return new KAdaptableInfo_wjUj();
}

KAdaptableInfo *KAdaptableInfo_wjUj::clone() const {
    return new KAdaptableInfo_wjUj(*this);
}

void KAdaptableInfo_wjUj::makeVars() {

    X.clear();
    Y.clear();

    // O
    X.addVarType("O", 'C', -CPX_INFBOUND, +CPX_INFBOUND, 1);
    X.setVarObjCoeff(1.0, "O", 0);

    // U
    X.addVarType("U", 'B', 0, 1, m_instance->jobs().size());

    // y
    Y.addVarType("y", 'B', 0, 1, m_instance->occurences().size());

    // z
    Y.addVarType("z", 'B', 0, 1, m_instance->occurences().size());

    // t
    Y.addVarType("t", 'B', 0, 1, m_instance->occurences().size());

}

void KAdaptableInfo_wjUj::makeUncSet() {
    U.clear();

    const unsigned int I = m_instance->jobs().size();

    // define uncertain risk factors
    for (unsigned int i = 0 ; i < I ; ++i) {
        U.addParam(0, 0, 1);
    }

    // define gamma constraint
    std::vector<std::pair<int, double> > constraint;
    for (int i = 1; i <= I; ++i) { constraint.emplace_back(i, 1); }
    U.addFacet(constraint, 'L', m_instance->gamma());
}

void KAdaptableInfo_wjUj::makeConsX() {

    // B_X
    B_X.clear();

    // LB
    for (const Job *job : m_instance->jobs()) {
        ConstraintExpression expr;
        expr.rowname("UB_x(" + std::to_string(job->id) + ")");
        expr.addTermX(getVarIndex_1("x", job->id), 1);
        expr.sign('G');
        expr.RHS(0);
        B_X.emplace_back(expr);
    }
    // UB
    for (const Job *job : m_instance->jobs()) {
        ConstraintExpression expr;
        expr.rowname("UB_x(" + std::to_string(job->id) + ")");
        expr.addTermX(getVarIndex_1("x", job->id), 1);
        expr.sign('L');
        expr.RHS(1);
        B_X.emplace_back(expr);
    }

    // C_X
    C_X.clear();

    // C_XQ
    C_XQ.clear();
}

void KAdaptableInfo_wjUj::makeConsY(unsigned int K) {

    if (K== 0) {
        B_Y.clear();
        C_XY.clear();
        C_XYQ.clear();
    }

    for (unsigned int k = B_Y.size() ; k <= K ; ++k) {

        if (B_Y.size() < k + 1) { B_Y.resize(k + 1); }

        B_Y[k].clear();

        for (const auto &var : {"y", "z", "t"}) {
            // LB
            for (const auto *occ : m_instance->occurences()) {
                ConstraintExpression expr;
                expr.addTermX(getVarIndex_2(k, var, occ->id), 1);
                expr.sign('G');
                expr.RHS(0);
                B_Y[k].emplace_back(expr);
            }
            // UB
            for (const auto *occ : m_instance->occurences()) {
                ConstraintExpression expr;
                expr.addTermX(getVarIndex_2(k, var, occ->id), 1);
                expr.sign('L');
                expr.RHS(1);
                B_Y[k].emplace_back(expr);
            }
        }
    }

    // C_XY
    for (unsigned int k = C_XY.size() ; k <= K ; ++k) {

        if (C_XY.size() < k + 1) { C_XY.resize(k + 1); }

        C_XY[k].clear();

        // TODO

    }

    // C_XYQ
    for (unsigned int k = C_XYQ.size() ; k <= K ; ++k) {

        if (C_XYQ.size() < k + 1) { C_XYQ.resize(k + 1); }

        C_XYQ[k].clear();

        ConstraintExpression expr;
        // TODO
        expr.addTermX(getVarIndex_1("O", 0), -1);
        expr.sign('L');
        expr.RHS(0);
        C_XYQ[k].emplace_back(expr);

    }

}

void KAdaptableInfo_wjUj::build() {
    hasInteger = true;
    objectiveUnc = false;
    existsFirstStage = false;
    numFirstStage = m_instance->jobs().size();
    numSecondStage = m_instance->occurences().size() * 3; // y, t, z
    numPolicies = 1;
    solfilename = "result.txt";

    makeUncSet();
    makeVars();
    makeConsX();
    makeConsY(0);

    assert(isConsistentWithDesign());
}
