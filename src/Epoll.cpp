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

        if (false == setNonblocking(m_iListenFd))
        {
            throw std::runtime_error("setNonBlocking() failed, error code: " + std::to_string(errno));
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

    Epoll::stop()
    {
        if (EPOLL_STATUS::START == m_iEpollStatus)
        {
            m_iEpollStatus = EPOLL_STATUS::STOP
        }
    }

    Epoll::doEpoll()
    {
        while (EPOLL_STATUS::START == m_iEpollStatus)
        {
            auto numFdReady = epoll_wait(m_iEpfd, m_pEvents, m_iNumEvents, 0);
            for (auto i = 0; i < numFdReady; i++) 
            {
                handleEpollEvent(m_pEvents[i])
            }
        }
    }

    void Epoll::handleEpollEvent(epoll_event &event)
    {
        if (event.data.fd == m_iListenFd)
        {

        }
        else if (event.events & EPOLLIN)
        {

        }
    }

    void Epoll::handleAcceptEvent(epoll_event &event)
    {
        sockaddr_in client_sin;
		socklen_t sin_size = sizeof(client_sin);
		ClientDescriptorType *client;

		int client_fd = accept(listen_fd_, reinterpret_cast<sockaddr *>(&client_sin), &sin_size);
		if(client_fd == -1)
		{
			printf("accept() failed, error code: %d\n", errno);
			return false;
		}

		if(!SetNonblocking(client_fd))
		{
			printf("failed to put fd into non-blocking mode, error code: %d\n", errno);
			return false;
		}

		//allocate and initialize a new descriptor for the client
		client = new ClientDescriptorType(client_fd, client_sin.sin_addr, 
										  ntohs(client_sin.sin_port), 
										  timeout_secs_);

		epoll_event ev;
		ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;	//client events will be handled in edge-triggered mode
		ev.data.ptr = client;						//we will pass client descriptor with every event

		if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) == 1)
		{
			printf("epoll_ctl() failed, error code: %d\n", errno);
			delete client;
			return false;
		}

		//store new client descriptor into the map of clients
		clients_[client_fd] = client;

		printf("[+] new client: %s:%d\n", inet_ntoa(client_sin.sin_addr), ntohs(client_sin.sin_port));
		return true;
    }

    bool Epoll::setNonblocking(int fd)
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