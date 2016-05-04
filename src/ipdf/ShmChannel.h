//
// Created by Ulrich Eck on 10/11/2015.
//

#ifndef IPDF_SHMCHANNEL_H
#define IPDF_SHMCHANNEL_H

#include "ipdf/ipdf.h"
#include "ipdf/ShmStream.h"

#include <memory>
#include <vector>


namespace ipdf {

class ShmChannel : boost::noncopyable {

public:
    ShmChannel(const char* port_name, const char* channel_name, const std::shared_ptr<bip::managed_shared_memory>& msm) :
        m_port_name(port_name),
        m_channel_name(channel_name),
        m_memory_manager(msm) {};

    bool allocate() {
        bool ret = true;
        for (std::vector<std::shared_ptr< ShmStreamBase > >::iterator it=m_streams.begin(); it!=m_streams.end(); ++it) {
            ret &= (*it)->allocate();
        }
        return ret;
    }

    bool deallocate() {
        bool ret = true;
        for (std::vector<std::shared_ptr< ShmStreamBase > >::iterator it=m_streams.begin(); it!=m_streams.end(); ++it) {
            ret &= (*it)->deallocate();
        }
        return ret;
    }

    std::string getName() {
        std::string c_name = m_port_name + std::string("_") + m_channel_name;
        return c_name;
    }

    unsigned int count() {
        return m_streams.size();
    }

    template< typename T, size_t N >
    std::shared_ptr<ShmStream<T, N> > addStream(const char* stream_name, bool is_buffered = false) {
        std::string c_name = getName();
        BOOST_LOG_TRIVIAL(debug) << "add Stream for component " << c_name.c_str();
        std::shared_ptr<ShmStream<T, N> > ret( new ShmStream<T, N>(c_name.c_str(), stream_name, m_memory_manager));
        m_streams.push_back(ret);
        return ret;
    }

    template< typename T, size_t N  >
    std::shared_ptr<ShmStream<T, N> > getChannel(unsigned int idx) {
        // bounds check..
        return std::dynamic_pointer_cast<ShmStream<T, N> >(m_streams.at(idx));
    }


private:
    const std::string m_port_name;
    const std::string m_channel_name;

    std::shared_ptr<bip::managed_shared_memory> m_memory_manager;
    std::vector<std::shared_ptr< ShmStreamBase > > m_streams;

};

} // end namespace

#endif //IPDF_SHMCHANNEL_H
