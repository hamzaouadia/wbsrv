
#ifndef RESPBUILDER_HPP
#define RESPBUILDER_HPP

#include "reqHandler.hpp"

class Response {
    public :
        int pipfd[2];
        bool folder;
        bool cgi_on;
        std::ifstream fl;
        int endOfResp;
        int cliSock;
        ReqHandler  *req;
        std::map<std::string, std::string> cType;
        Response( ReqHandler *_req, int _cliSock );
        ~Response();
        void    getMethod();
        void    exute_cgi(std::stringstream& response);
        bool    is_cgi();
        std::string  read_from_a_file();
        std::string  list_folder();
        std::string ret_folder();
        std::string get_file_ext(std::string path);
        std::string getHdResp();
};

#endif