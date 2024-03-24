
#ifndef REQHANDLER_HPP
#define REQHANDLER_HPP

#include "fconf.hpp"
#include <sys/socket.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>

typedef struct reqFinalForm {
    int         status;
    std::string method;
    std::string uri;
} s_reqFinalForm;

class ReqHandler {
    
    public:
    /*tests*/
        // std::map<int, sockaddr_in> socknData;
        // std::vector<Serv> servers;
        std::vector<std::string>  reqHds;
        std::string body_string;
        std::string non_body_str;
        std::string value;
        // std::string srvName;
        // std::string hostName;
        // int fdServ;
        bool    passedOnce;
    /*end tests*/
        int                                 endOfRead;
        unsigned long                       content_lenght;
        unsigned long                       bytes_red;
        bool                                bodyStartFound;
        // int                                 client_sock;
        int                                 loc_idx;
        
        Serv                                myServ;
        s_reqFinalForm                      request;
        std::vector<std::string>            req;
        std::map<std::string, std::string>  reqHeaders;

        // methods
        ReqHandler();
        // ReqHandler( std::map<int, sockaddr_in> &_socknData, int _fdServ, std::vector<Serv> &_servers );
        ReqHandler( Serv &_myServ );
        ~ReqHandler(){ std::cout << "destructor requesthandler is called" << std::endl;}
        void        checkBuff( char *buff, size_t bytes );
        void        nextBuff( char *buff );
        void        reqStrToVec( std::string &line );
        void        fillReqHeaders();
        // void    getRequestFinalForm();
        void        countBodyBytes( std::string &str );
        void        parse_request();
        void        parseHeaders();
        int         getHeaderVal( std::string key, std::string &val );
        void        uri_depon_cs( int code );
        void        getFinalUri( std::string str );
        void        fileOrDir();
        void        checkRetnMeth( s_location &loc, std::vector<std::string> &splited_uri, int i );
        int         isLocation( std::string &str );
        s_location  getLocationByName( std::string &str );
        std::string getFullUri( std::vector<std::string> &spl_uri, std::string &root, int i );
        // Serv    getServer();
};

#endif