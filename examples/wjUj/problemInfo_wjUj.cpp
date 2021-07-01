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

}

void KAdaptableInfo_wjUj::makeUncSet() {

}

void KAdaptableInfo_wjUj::makeConsX() {

}

void KAdaptableInfo_wjUj::makeConsY(unsigned int k) {

}

void KAdaptableInfo_wjUj::build() {
    throw std::runtime_error("build() not implemented");
}
