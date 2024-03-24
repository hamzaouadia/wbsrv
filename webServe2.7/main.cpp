

#include <cstring>
#include "includes/multiPlex.hpp"

void    lcs_num( std::vector<Serv> servers )
{
    for ( size_t i = 0 ; i < servers.size() ; i++ )
    {
        int n = 0;
        for ( size_t j = 0 ; j < servers[i].locations.size() ; j++ )
        {
            // std::cout << servers[i].locations[j].get << "   == GET" << std::endl;
            n++;
        }
        std::cout << "server " << i + 1 << " has : " << n << " location" << std::endl;
    }
}

int main( int ac, char **av )
{
    std::vector<Serv> servers;
    if ( ac > 2 )
        std::cerr << "wrong number of arguments!" << std::endl;
    else if ( ac == 2 )
    {
        // std::cout << "here 1" << std::endl;
        Fconf   configFile( av[1] );
        servers = configFile.getServers();
        lcs_num ( servers );
        // std::cout << "first server locations size : " << servers[0].locations.size() << std::endl;
        // std::cout << "second server locations size : " << servers[1].locations.size() << std::endl;
        if ( !servers.size() )
        {
            std::cerr << "No correct server found, re-check your config file" << std::endl;
            std::cout << "the default one will be used" << std::endl;
        }
        MultiPlexer mplex( servers );
        mplex.webServLoop( servers );
    }


    // std::vector<int> socks;
    // int epollFd = epoll_create( servers.size() );
    // std::cout << "epoll fd = " << epollFd << std::endl;
    // if ( epollFd == -1 )
    // {
    //     std::cout << "failed epoll creation" << std::endl;
    //     exit (1);
    // }
    // int i = 1;
    // for ( std::vector<serv>::iterator it = servers.begin() ; it != servers.end() ; it++ ) {

    //     int sock = socket( AF_INET, SOCK_STREAM, 0 );
    //     std::cout << "socket number :"<< sock << std::endl;
    //     int f = fcntl(sock, F_GETFL, 0);
    //     fcntl(sock, F_SETFL, f | O_NONBLOCK);
    //     int reuse = 1;
    //     if ( setsockopt( sock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse) ) )
    //     {
    //         std::cerr << "setsockopt error" << std::endl;
    //         close(sock);
    //         return 1;
    //     }
    //     sockaddr_in servAdd;
    //     servAdd.sin_family = AF_INET;
    //     servAdd.sin_addr.s_addr = INADDR_ANY;
    //     servAdd.sin_port = htons( it->port );
    //     bind( sock, (struct sockaddr*)&servAdd, sizeof( servAdd ) );
    //     listen( sock, SOMAXCONN );
    //     std::cout << "server : " << it->servName << " listen to port : " << it->port << std::endl;
    //     socks.push_back( sock );

    //     struct epoll_event ev;
    //     ev.events = EPOLLIN | EPOLLOUT;
    //     ev.data.fd = sock;

    //     if ( epoll_ctl( epollFd, EPOLL_CTL_ADD, sock, &ev ) == -1 )
    //     {
    //         std::cout << "failed to add sock to epoll" << std::endl;
    //         close(epollFd);
    //         exit(1);
    //     }
    //     i++;
    // }

    // struct epoll_event evs[servers.size()];
    // std::map<int, int> servCli;
    // while (1)
    // {
    //     std::cout << "Im before epoll wait" << std::endl;
    //     int evNum = epoll_wait( epollFd, evs, servers.size(), -1 );
    //     if ( evNum == -1 )
    //     {
    //         std::cout << "error epoll wait" << std::endl;
    //         break;
    //     }
    //     for ( int i = 0 ; i < evNum ; i++ )
    //     {
    //         if ( evs[i].events & EPOLLIN )
    //         {
    //             std::cout << "event found on fd: " << evs[i].data.fd << std::endl;
    //             int cliSock = accept( evs[i].data.fd, NULL, NULL);
    //             if ( cliSock == -1 )
    //                 std::cout << "ERROR ACCEPT FUNCTIONNNN" << std::endl;
    //             else
    //             {
    //                 servCli[evs[i].data.fd] = cliSock;
                    
    //                         //add
    //                 struct epoll_event ev;
    //                 ev.events = EPOLLIN | EPOLLOUT;
    //                 ev.data.fd = cliSock;
    //                 if ( epoll_ctl( epollFd, EPOLL_CTL_ADD, cliSock, &ev ) == -1 )
    //                 {
    //                     std::cout << "failed to add sock to epoll" << std::endl;
    //                     close(epollFd);
    //                     exit(1);
    //                 }
    //                 else
    //                     std::cout << "client Socket added to EPOLL succesfuly !!!" << std::endl;
    //                         //end of add
    //                 std::cout << "accepted connection on :" << cliSock << std::endl;
    //                 std::string res = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 22\r\nConnection: keep-alive\r\n\r\n<div>hello world</div>";
    //                 const char *ress = res.c_str();
    //                 std::cout << "here" << std::endl;
    //                 size_t s = send(cliSock, ress, strlen(ress), 0);
    //                 if ( s == res.size() )
    //                 {
    //                     std::cout << "all sent" << std::endl;
    //                     if ( epoll_ctl( epollFd, EPOLL_CTL_DEL, cliSock, &evs[i] ) == -1 )
    //                         std::cout << "failed deleted from epoll" << std::endl;
    //                     else
    //                         std::cout << "fd deleted from epoll" << std::endl;
    //                     close(cliSock);
    //                 }
    //             }
    //             std::cout << "Im after accept" << std::endl;
                   
    //         }
    //         else if ( evs[i].events & EPOLLOUT )
    //         {
    //                 std::cout << "im epolout" << std::endl;
    //         }
    //     }
    //         // return 0;
    // }
    // close(epollFd);
    // for ( std::vector<int>::iterator it = socks.begin() ; it != socks.end() ; it++ )
    // {
    //     close( *it );
    // }
}