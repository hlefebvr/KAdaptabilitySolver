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
    Y.addVarType("t", 'C', 0, +CPX_INFBOUND, m_instance->occurences().size());

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
        expr.addTermX(getVarIndex_1("U", job->id), 1);
        expr.sign('G');
        expr.RHS(0);
        B_X.emplace_back(expr);
    }
    // UB
    for (const Job *job : m_instance->jobs()) {
        ConstraintExpression expr;
        expr.rowname("UB_x(" + std::to_string(job->id) + ")");
        expr.addTermX(getVarIndex_1("U", job->id), 1);
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

        for (const auto &var : {"y", "z"}) {
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

        // LB for t_k
        for (const auto *occ : m_instance->occurences()) {
            ConstraintExpression expr;
            expr.addTermX(getVarIndex_2(k, "t", occ->id), 1);
            expr.sign('G');
            expr.RHS(0);
            B_Y[k].emplace_back(expr);
        }
        // UB for t_k
        for (const auto *occ : m_instance->occurences()) {
            ConstraintExpression expr;
            expr.addTermX(getVarIndex_2(k, "t", occ->id), 1);
            expr.sign('L');
            expr.RHS(occ->deadline);
            B_Y[k].emplace_back(expr);
        }
    }

    // C_XY
    for (unsigned int k = C_XY.size() ; k <= K ; ++k) {

        if (C_XY.size() < k + 1) { C_XY.resize(k + 1); }

        C_XY[k].clear();

        // \sum_{k|J_k\in G_j} y_k <= 1 - U_j
        std::vector<ConstraintExpression> constraints;
        constraints.resize(m_instance->jobs().size());

        for (const auto& job : m_instance->jobs()) {
            constraints[job->id].addTermX(getVarIndex_1("U",job->id), 1);
            constraints[job->id].sign('L');
            constraints[job->id].RHS(1);
        }

        for (const auto& occurrence : m_instance->occurences()) {
            constraints[occurrence->parent_job_id].addTermX(getVarIndex_2(k, "y", occurrence->id), 1);
        }

        for (auto&& expr : constraints) {
            C_XY[k].emplace_back(expr);
        }

        // t_k <= d_k
        for (const auto& occurrence : m_instance->occurences()) {
            ConstraintExpression expr;
            expr.addTermX(getVarIndex_2(k, "t", occurrence->id), 1);
            expr.sign('L');
            expr.RHS(occurrence->deadline);
            C_XY[k].emplace_back(std::move(expr));
        }

        // z_k <= y_k
        for (const auto& occurrence : m_instance->occurences()) {
            ConstraintExpression expr;
            expr.addTermX(getVarIndex_2(k, "z", occurrence->id), 1);
            expr.addTermX(getVarIndex_2(k, "y", occurrence->id), -1);
            expr.sign('L');
            expr.RHS(0);
            C_XY[k].emplace_back(std::move(expr));
        }

        // t_k - t_{k-1} - p_ky_k - tau_k z_k >= 0
        for (unsigned int i = 0, n = m_instance->occurences().size() ; i < n ; ++i) {
            const auto& occurrence = m_instance->occurences().at(i);
            ConstraintExpression expr;
            expr.addTermX(getVarIndex_2(k, "t", occurrence->id), 1);
            if (i > 0) {
                expr.addTermX(getVarIndex_2(k, "t", m_instance->occurences().at(i - 1)->id), -1);
            }
            expr.addTermX(getVarIndex_2(k, "y", occurrence->id), -1.0 * occurrence->processing_time);
            expr.addTermX(getVarIndex_2(k, "z", occurrence->id), -1.0 * occurrence->additional_time);
            expr.sign('G');
            expr.RHS(0);
            C_XY[k].emplace_back(std::move(expr));
        }

        // t_k - p_ky_k - tau_k z_k - M_k y_k >= r_k - M_k
        for (const auto& occurrence : m_instance->occurences()) {
            const double big_M = m_instance->max_deadline();
            ConstraintExpression expr;
            expr.addTermX(getVarIndex_2(k, "t", occurrence->id), 1);
            expr.addTermX(getVarIndex_2(k, "y", occurrence->id), -1.0 * (occurrence->processing_time + big_M));
            expr.addTermX(getVarIndex_2(k, "z", occurrence->id), -1.0 * occurrence->additional_time);
            expr.sign('G');
            expr.RHS(occurrence->release_date + (-1.0) * big_M);
            C_XY[k].emplace_back(std::move(expr));
        }

    }

    // C_XYQ
    for (unsigned int k = C_XYQ.size() ; k <= K ; ++k) {

        if (C_XYQ.size() < k + 1) { C_XYQ.resize(k + 1); }

        C_XYQ[k].clear();

        ConstraintExpression expr;
        double rhs = 0.0;
        expr.addTermX(getVarIndex_1("O", 0), -1);
        for (const auto& job : m_instance->jobs()) {
            expr.addTermX(getVarIndex_1("U", job->id), job->weight + -1.0 * job->outsourcing_cost);
            rhs += job->outsourcing_cost;
        }
        for (const auto& occurrence : m_instance->occurences()) {
            expr.addTermX(getVarIndex_2(k, "y", occurrence->id), -1.0 * occurrence->outsourcing_cost);
            expr.addTermProduct(getVarIndex_2(k, "y", occurrence->id), U.getParamIndex('q', occurrence->parent_job_id)+1, occurrence->max_degradation);
            expr.addTermProduct(getVarIndex_2(k, "z", occurrence->id), U.getParamIndex('q', occurrence->parent_job_id)+1, -1.0 * occurrence->max_degradation);
        }
        expr.sign('L');
        expr.RHS(-1.0 * rhs);
        C_XYQ[k].emplace_back(expr);

    }

}

void KAdaptableInfo_wjUj::build() {
    hasInteger = true;
    objectiveUnc = true;
    existsFirstStage = true;
    numFirstStage = m_instance->jobs().size() + 1; // U, O
    numSecondStage = m_instance->occurences().size() * 3; // y, t, z
    numPolicies = 1;
    solfilename = "result.txt";

    makeUncSet();
    makeVars();
    makeConsX();
    makeConsY(0);

    assert(isConsistentWithDesign());
}
