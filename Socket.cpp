#include <bits/stdint-intn.h>
#include <cstdint>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#if defined (DEBUG) && (DEBUG) > 0
   #include <cstdio>
#endif

// 1 kiB
constexpr int32_t READ_BUFFER_SIZE = 1024;

#include "Socket.hpp"
#include "IONotification.hpp"

using namespace std;
using namespace ramboindustries;

TCPSocket::TCPSocket():created_date(10){
   this->socket_fd = -1;
}

TCPSocket::~TCPSocket()
{
   if (this->socket_fd != -1)
   {
      close(this->socket_fd);
   }
}

uint64_t TCPSocket::get_created_date() const
{
   return this->created_date;
}

void TCPSocket::write_message(const int32_t other, const string message) const
{
   write(other, message.c_str(), message.size());
}

const string TCPSocket::read_message(const int32_t other) const
{
   string result = "";
   int32_t bytes_read = 0;
   do
   {
      char buffer[READ_BUFFER_SIZE] = {0};
      bytes_read = read(other, buffer, READ_BUFFER_SIZE);
      result.append(buffer);
   } while(bytes_read == READ_BUFFER_SIZE);
   return result;
}

ServerSocketImpl::ServerSocketImpl(
   const std::uint16_t max_pending_queue,
   const std::uint16_t port
) : max_pending_queue(max_pending_queue), port(port)
{
   this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

   struct sockaddr_in server;
   server.sin_family = AF_INET;

   server.sin_port = htons(port);
   server.sin_addr.s_addr = INADDR_ANY;

   if(bind(this->socket_fd,  (struct sockaddr *) &server, sizeof(server)) == -1)
   {
      close(this->socket_fd);
      throw "Could not bind socket to port: " + to_string(port);
   }

   if (listen(this->socket_fd, max_pending_queue) == -1)
   {
      close(this->socket_fd);
      throw "Could not set socket to passive mode with backlog of: " + to_string(this->max_pending_queue);
   }

   #if defined (DEBUG) && (DEBUG) > 0
      printf(
         "[%s - %s:%d] Socket Server is up and running on port: %d with queue pending size of: %d\n",
         __FILE__,
         __FUNCTION__,
         __LINE__,
         this->port,
         this->max_pending_queue
      );
   #endif
}

void ServerSocketImpl::run()
{
   this->running = true;

   /*
    * Register our server socket to the interest list
    * Kernel will notify us when a new client connected
   */
   epoll.add(this->socket_fd, READ_OPS);

   while (running)
   {
      for (auto& event : epoll.wait())
      {
         const auto event_fd = event.data.fd;
         if (event_fd == this->socket_fd)
         {
            // new connection received
            struct sockaddr_in client_address;
            socklen_t length = sizeof(client_address);
            int32_t client_fd = accept(socket_fd, (struct sockaddr *) &client_address, &length);

            // adds the new connected client to the epoll interest list
            // with this interest, we will be notified when the client socket has sent a message
            epoll.add(client_fd, READ_OPS);

            #if defined (DEBUG) && (DEBUG) > 0
               printf(
                  "[%s - %s:%d] New connection %d added to epoll interests set\n",
                  __FILE__,
                  __FUNCTION__,
                  __LINE__,
                  client_fd
               );
            #endif
         }
         else
         {
            #if defined (DEBUG) && (DEBUG) > 0
               printf(
                  "[%s - %s:%d] The file descriptor %d has sent a message!\n",
                  __FILE__,
                  __FUNCTION__,
                  __LINE__,
                  event_fd
               );
            #endif
            const auto message = read_message(event_fd);
            const auto message_to_send = "HTTP/1.1 200 OK\n\r\n\r<html><head><strong><i>Sending back you message!</i></strong>/</head><body><p>" + message + "</p></body></html>";
            write_message(event_fd, message_to_send);

            // TODO: Create and close connections is wastefull
            // Would be so handful to not close the connection after we process the event
            epoll.remove(event_fd);
            close(event_fd);
         }
      }
   }

}

void ServerSocketImpl::stop()
{
   this->running = false;
}