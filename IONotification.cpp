#include <cstdint>
#include <sys/epoll.h>
#include <unistd.h>

#include "IONotification.hpp"

#if defined (DEBUG) && (DEBUG) > 0
   #include <cstdio>
#endif

using namespace ramboindustries;
using namespace std;

Epoll::Epoll()
{
   this->epoll_fd = epoll_create1(0);
   #if defined (DEBUG) && DEBUG > 0
      printf(
         "[%s - %s:%d] Epoll created with file descriptor: %d\n",
         __FILE__,
         __FUNCTION__,
         __LINE__,
         this->epoll_fd
      );
   #endif
}


Epoll::~Epoll()
{
   if (this->epoll_fd != -1)
   {
      close(epoll_fd);
   }
   #if defined (DEBUG) && DEBUG > 0
      printf(
         "[%s - %s:%d] Epoll with file descriptor: %d closed!\n",
         __FILE__,
         __FUNCTION__,
         __LINE__,
         this->epoll_fd
      );
   #endif
}

void Epoll::add(int32_t fd, const EpollFlags flags) const
{
   struct epoll_event event;
   event.data.fd = fd;
   event.events = flags;
   epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, fd, &event);

   #if defined (DEBUG) && DEBUG > 0
      printf(
         "[%s - %s:%d] Epoll added interest to file descriptor: %d with interests in events: %d\n",
         __FILE__,
         __FUNCTION__,
         __LINE__,
         this->epoll_fd,
         flags
      );
   #endif
}

void Epoll::remove(const int32_t fd) const
{
   epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
   #if defined (DEBUG) && DEBUG > 0
      printf(
         "[%s - %s:%d] Epoll removed interest to file descriptor %d\n",
         __FILE__,
         __FUNCTION__,
         __LINE__,
         this->epoll_fd
      );
   #endif
}

vector<struct epoll_event> Epoll::wait() const
{
   vector<struct epoll_event> notifications;
   auto events = new struct epoll_event[EPOLL_MAX_EVENTS];
   int32_t number_fds_notification = epoll_wait(this->epoll_fd, events, EPOLL_MAX_EVENTS, -1);
   for (auto index = 0; index < number_fds_notification; index++)
   {
      notifications.push_back(events[index]);
   }
   return notifications;
}
