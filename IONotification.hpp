#pragma once

#include <cstdint>
#include <sys/epoll.h>
#include <vector>

constexpr std::uint8_t EPOLL_MAX_EVENTS = 10;

namespace ramboindustries {

   enum EpollFlags
   {
      READ_OPS       = EPOLLIN,
      WRITE_OPS      = EPOLLOUT,
      READ_WRITE_OPS = EPOLLIN | EPOLLOUT,
      ALL_OPS        = EPOLLIN | EPOLLOUT,
   };


   class Epoll
   {
      private:
         std::int32_t epoll_fd;
      public:
         Epoll();
         ~Epoll();
         void add(const std::int32_t fd, const EpollFlags flags) const;
         void remove(const std::int32_t fd) const;
         std::vector<struct epoll_event> wait() const;
   };

};