#pragma once

#include <cstdint>
#include <string>

#include "IONotification.hpp"
namespace ramboindustries
{

   class ClientSocketImpl;
   class ServerSocketImpl;

   class TCPSocket
   {
      public:
         TCPSocket();
         ~TCPSocket();
      private:
         const std::uint64_t created_date;
      protected:
         std::int32_t socket_fd;
         std::uint64_t get_created_date() const;

         void write_message(const std::int32_t other, const std::string message) const;
         const std::string read_message(const std::int32_t other) const;

   };


   class ServerSocketImpl : public TCPSocket
   {
      public:
         ServerSocketImpl(
            const std::uint16_t max_pending_queue,
            const std::uint16_t port
         );
         void run();
         void stop();
      private:
         bool running;
         const std::uint16_t max_pending_queue;
         const std::uint16_t port;
         Epoll epoll;

   };

};