//
// Created by Ulrich Eck on 28/10/2015.
//

#ifndef IPDF_SHMPORTCONDITION_H
#define IPDF_SHMPORTCONDITION_H

#include "ipdf/ipdf.h"

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

namespace ipdf {

class ShmPortCondition {

public:
    ShmPortCondition() : data_available(false) {};

    bip::interprocess_mutex      port_mutex;
    bip::interprocess_condition  port_condition;
    bool data_available;
};

} // end namespace

#endif //IPDF_SHMPORTCONDITION_H
