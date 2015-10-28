//
// Created by Ulrich Eck on 28/10/2015.
//

#ifndef IPDF_SHMPORT_H
#define IPDF_SHMPORT_H

#include "ipdf/ipdf.h"

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

class ShmPort {

public:
    ShmPort() : data_available(false) {};

    bip::interprocess_mutex      port_mutex;
    bip::interprocess_condition  port_condition;
    bool data_available;
};

#endif //IPDF_SHMPORT_H
