/*
 * File Name:  Epoll.cpp
 * 
 * Created By Zed Lee 
 * Created On 2019-01-05 22:51:09 
 * 
 * Last Modified by Zed Lee 
 * Last Modified time: 2019-01-05 22:51:09 
 *
 */

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#include "Epoll.h"

namespace yasuo
{
    Epoll::Epoll()
    {
		sockaddr_in sin = { 0 };

		sin.sin_addr.s_addr = inet_addr(this->m_strIP.c_str());
		sin.sin_family = AF_INET;
		sin.sin_port = htons(this->m_iPort);

		m_iListenFd = socket(AF_INET, SOCK_STREAM, 0);
		if (0 >= m_iListenFd)
        {
			throw std::runtime_error("socket() failed, error code: " + std::to_string(errno));
        }

		if (bind(m_iListenFd, reinterpret_cast<sockaddr *>(&sin), sizeof(sin)))
        {
			throw std::runtime_error("bind() failed, error code: " + std::to_string(errno));
        }

		if (false == SetNonblocking(m_iListenFd))
        {
			throw std::runtime_error("SetNonBlocking() failed, error code: " + std::to_string(errno));
        }

		if (-1 == listen(m_iListenFd, SOMAXCONN))
        {
			throw std::runtime_error("listen() failed, error code: " + std::to_string(errno));
        }

		m_iEpfd = epoll_create1(0);
		if (-1 == m_iEpfd)
        {
			throw std::runtime_error("epoll_create1() failed, error code: " + std::to_string(errno));
        }

		epoll_event e_event;
		e_event.events = EPOLLIN;
		e_event.data.fd = m_iListenFd;

		if (-1 == epoll_ctl(m_iEpfd, EPOLL_CTL_ADD, m_iListenFd, &e_event))
        {
			throw std::runtime_error("epoll_ctl() failed, error code: " + std::to_string(errno));
        }

    };

    Epoll::~Epoll()
    {
        if (-1 != m_iEpfd) 
        {
            close(m_iEpfd);
        }
        if (-1 != m_iListenFd) 
        {
            close(m_iListenFd);
        }
    }

    Epoll::start()
    {
        if (EPOLL_STATUS::STOP == m_iEpollStatus)
        {
            m_iEpollStatus = EPOLL_STATUS::START
            doEpoll();
        }
    }

    Epoll::doEpoll()
    {
        while (EPOLL_STATUS::START == m_iEpollStatus)
        {
            int numFdReady = epoll_wait(m_iEpfd, m_pEvents, m_iNumEvents, 0);
            for (int i = 0; i < numFdReady; i++) 
            {
                Poll *poll = (Poll *) readyEvents[i].data.ptr;
                int status = -bool(readyEvents[i].events & EPOLLERR);
                callbacks[poll->state.cbIndex](poll, status, readyEvents[i].events);
            }
        }
    }

    bool Epoll::SetNonblocking(int fd)
	{
		int flags = fcntl(fd, F_GETFL, 0);
		if (-1 == flags)
			return false;

		flags |= O_NONBLOCK;

		if (-1 == fcntl(fd, F_SETFL, flags))
			return false;

		return true;
	}
}