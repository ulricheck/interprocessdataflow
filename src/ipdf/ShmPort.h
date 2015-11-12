//
// Created by Ulrich Eck on 10/11/2015.
//

#ifndef IPDF_SHMPORT_H
#define IPDF_SHMPORT_H

#include "ipdf/ipdf.h"
#include "ipdf/ShmChannel.h"
#include "ipdf/ShmPortCondition.h"

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#include <string>
#include <vector>

namespace ipdf {

class ShmPort : boost::noncopyable {

public:
    ShmPort(const char* port_name, const std::shared_ptr<bip::managed_shared_memory>& msm) :
            m_port_name(port_name),
            m_memory_manager(msm),
            m_port_condition(nullptr) {};

    bool allocate() {
        try {
            std::string pc_name = getName();
            pc_name.append("_condition");
            m_port_condition = m_memory_manager->find_or_construct<ShmPortCondition>(pc_name.c_str())();
            bool ret = true;
            for (auto &e : m_channels) {
                ret &= e->allocate();
            }
            return ret;
        } catch (bip::bad_alloc &e) {
            BOOST_LOG_TRIVIAL(error) << "error while allocating ShmPortCondition for: " << getName() << " - " << e.what();
        }
        return false;
    }

    bool deallocate() {
        bool ret = true;
        for (auto &e : m_channels) {
            ret &= e->deallocate();
        }
        if (m_port_condition) {
            m_memory_manager->destroy_ptr(m_port_condition);
            m_port_condition = nullptr;
        }
        return ret;
    }

    std::string getName() {
        return std::string(m_port_name);
    }

    unsigned int count() {
        return m_channels.size();
    }

    int addChannel(const char* channel_name) {
        m_channels.push_back(std::make_shared<ShmChannel>(getName().c_str(), channel_name, m_memory_manager));
        return m_channels.size() - 1;
    }

    std::shared_ptr<ShmChannel> getChannel(unsigned int idx) {
        // bounds check..
        return m_channels.at(idx);
    }

private:
    const char* m_port_name;
    std::shared_ptr<bip::managed_shared_memory> m_memory_manager;
    std::vector<std::shared_ptr< ShmChannel > > m_channels;

    ShmPortCondition* m_port_condition;

};

} // end namespace

#endif //IPDF_SHMPORT_H
