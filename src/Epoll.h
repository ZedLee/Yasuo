/*
 * File Name:  Epoll.h
 * 
 * Created By Zed Lee 
 * Created On 2019-01-05 22:51:09 
 * 
 * Last Modified by Zed Lee 
 * Last Modified time: 2019-01-05 22:51:09 
 *
 */
#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <string>

namespace yasuo
{

    class Epoll 
    {
    public:
        Epoll(std::string ip, int port) : m_strIP(ip), m_iPort(port) { Epoll(); };

        Epoll();
       
        ~Epoll();

        int start();

        int stop();

    protected:
        // Set a file descriptor as non blocking mode
    	bool SetNonblocking(int fd);

        // Do epoll
        void doEpoll();

    private:

        // epoll file description
        int m_iEpfd;

        // amount of events
        const int m_iNumEvents = 1024;

        // Events that needed to process
        epoll_event m_pEvents[m_iNumEvents];

        // Port that needed to listen
        int m_iPort = 80;

        // IP address needed to bind
        std::string m_strIP = "0.0.0.0"

        int m_iListenFd;

        enum EPOLL_STATUS {
            STOP = 0,
            START
        }

        int m_iEpollStatus = EPOLL_STATUS::STOP;

    };

}

#endif // EPOLL_H