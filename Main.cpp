#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

#include "Socket.hpp"

using namespace std;
using namespace ramboindustries;


void on_conn(int32_t client_id)
{
   cout << "Hello client: " << to_string(client_id) << endl;
   sleep(3);
}


int main(int argc, char **argv)
{

   ServerSocketImpl server(10, 8080);
   server.run();




   return EXIT_SUCCESS;
}