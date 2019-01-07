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

namespace dWS
{

    class Epoll 
    {
    public:
        Epoll();
       
        ~Epoll();

        int startEpoll();

    private:

        // epoll file description
        int m_iEpfd;

        // max connections
        int m_iMaxConn = 1024000;

        // amount of events
        int m_iNumEvents = 1024;

    };

}

#endif // EPOLL_H