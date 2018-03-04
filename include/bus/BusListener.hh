//
// Created by FyS on 29/08/17.
//

#ifndef FREESOULS_BUSLISTENER_HH
#define FREESOULS_BUSLISTENER_HH

#include <spdlog/spdlog.h>
#include <zconf.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <QueueContainer.hh>


namespace fys::mq {

    /**
     * MsgType : Message type gotten from the queue listened in the Bus
     * Functor : Functor to execute for the message listened
     *           It has to implement its () function plus having a public IndexInBus enum hack
     *
     */
    template <typename Functor, typename BusType>
    class BusListener {

    public:
        ~BusListener() = default;
        explicit BusListener(Functor func) : _indexInBus(Functor::IndexInBus), _functor(func) {
            static_assert(Functor::IndexInBus >= 0);
        }

        void launchListenThread(typename BusType::ptr bus, const bool launchTread = true) {
            if (launchTread) {
                boost::thread thread([this, &bus]() {
                    this->listen(bus);
                });
                thread.detach();
            }
            else
                listen(bus);
        }

    private:
        void listen(typename BusType::ptr bus) {
            if (!bus->isIndexQueueLegitimate(_indexInBus)) {
                std::cerr << "Listener couldn't be launched; the index (" << _indexInBus << ") in bus is not legitimate" << std::endl;
                return;
            }
            spdlog::get("c")->debug("Listener launched, listen on queue: {} for functor: {}", _indexInBus, typeid(_functor).name());
            while (true) {
                auto msgContainer = bus->popFromBus(_indexInBus);
                if (static_cast<bool>(msgContainer))
                    _functor(msgContainer.value());
            }
        }

    private:
        u_int _indexInBus;
        Functor _functor;

    };

}

#endif //FREESOULS_BUSLISTENER_HH
