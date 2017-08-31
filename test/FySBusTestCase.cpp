//
// Created by FyS on 16/06/17.
//

#define BOOST_TEST_MODULE test_bus

#include <boost/test/unit_test.hpp>
#include <bus/FysBus.hh>
#include <boost/thread.hpp>
#include <thread>

/**
 * ------------------------------------------------------
 *  Bus Initialisation test------------------------------
 * ------------------------------------------------------
 */

BOOST_AUTO_TEST_CASE( test_bus_ini )
{
    fys::mq::FysBus<std::string, 100> bus("/home/FyS/ClionProjects/FySMQ/docs/test_bus_inifile.ini");
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( test_bus_ini_error1, 1 )
BOOST_AUTO_TEST_CASE( test_bus_ini_error1 )
{
    fys::mq::FysBus<std::string, 100> bus("/home/FyS/ClionProjects/FreeYourSoul_MQ/docs/test_bus_inifile_error1.ini");
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( test_bus_ini_error2, 1 )
BOOST_AUTO_TEST_CASE( test_bus_ini_error2 )
{
    fys::mq::FysBus<std::string, 100> bus("/home/FyS/ClionProjects/FySMQ/docs/test_bus_inifile_error2.ini");
}

/**
 *------------------------------------------------------
 * Bus exections test ----------------------------------
 *------------------------------------------------------
 */
fys::mq::LockFreeQueue<fys::mq::QueueContainer<std::string>, 5000> *lockFreeQueue;
int readValues;

void addinlockfreequeue(const fys::mq::QueueContainer<std::string> &container) {

    for (int i = 0; i < 1500; ++i) {
        lockFreeQueue->push(container);
    }
}

void readLockFreeQueue() {
    for (int i = 0; i < 10000; ++i) {
        fys::mq::QueueContainer<std::string> *container;

        container = lockFreeQueue->pop();
        usleep(100);
        if (container != NULL) {
            ++readValues;
//            std::cout << readValues << " | ";
        }
    }
}

void initTestExecution() {
    lockFreeQueue = new fys::mq::LockFreeQueue<fys::mq::QueueContainer<std::string>, 5000>();
    lockFreeQueue->setLockingWhenEmpty(false);
    readValues = 0;
}

void cleanTestExecution() {
    if (lockFreeQueue != nullptr) {
        delete(lockFreeQueue);
        lockFreeQueue = nullptr;
    }
}

BOOST_AUTO_TEST_CASE( test_bus_exec ) {
    initTestExecution();
    std::cout << "begin test exec" << std::endl;
    fys::mq::QueueContainer<std::string> c1("A");
    fys::mq::QueueContainer<std::string> c2("B");
    fys::mq::QueueContainer<std::string> c3("C");
    std::thread workerRead(readLockFreeQueue);
    std::thread w1(addinlockfreequeue, c1);
    std::thread w2(addinlockfreequeue, c2);
    std::thread w3(addinlockfreequeue, c3);

    sleep(2);
    w1.join();
    w2.join();
    w3.join();
    workerRead.join();
    cleanTestExecution();
    std::cout << "end test exec : readValue = " << readValues << std::endl;
    BOOST_ASSERT(readValues == 4500);
}