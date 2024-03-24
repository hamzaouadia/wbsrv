
#include "includes/respBuilder.hpp"


std::string Response::ret_folder()
{
    std::vector<std::string> rt;
    if ( req->loc_idx == -1 )
        rt = split_uri( req->myServ.root );
    else
        rt = split_uri( req->myServ.locations[req->loc_idx].root );
    size_t pos = req->request.uri.rfind(rt.back());
    std::string fldr = req->request.uri.substr(pos + rt.back().size(), req->request.uri.size());
    fldr == "" ? fldr : fldr += "/";
    return fldr;
}

std::string     Response::list_folder()
{
    std::stringstream response;
    std::string old_p;
    struct dirent *entry;
    DIR *dir = opendir(req->request.uri.c_str());

    if (dir == NULL)
      return "";
    response << "<!DOCTYPE html>";
    response << "<html>";
    response << "<head>";
    response << "<title> listing a folder </title>";
    response << "</head>";
    response << "<body>";
    while ((entry = readdir(dir)) != NULL)
    {
        // std::cerr << old_p << std::endl;
        response << "<div>";
        response << "<a href=\"";
        if (req->loc_idx != -1)
            response << req->myServ.locations[req->loc_idx].name << "/";
        response << old_p << entry->d_name << "\">" << entry->d_name << "</a>";
        response << "</div>";
        old_p = ret_folder();
    }
    response << "</body>";
    closedir(dir);
    endOfResp = 1;
    return response.str();
}

#include<stdlib.h> 
#include<unistd.h> 
#include<stdio.h> 
#include<fcntl.h>

void    Response::exute_cgi(std::stringstream& response)
{
    char buffer[1024];
    ssize_t bytesRead;
    memset(buffer, 0, 1024);
    if ((bytesRead = read(pipfd[0], buffer, sizeof(buffer))) == -1)
    {
        if (errno == EBADF)
        {
            if (pipe(pipfd) == -1)
            {
                std::cerr << "Failed to create pipe" << std::endl;
                return;
            }
            pid_t c_pid = fork(); 
        
            if (c_pid == -1)
                return ;
            else if (c_pid == 0)
            {
                close(pipfd[0]);
                dup2(pipfd[1], STDOUT_FILENO);
                dup2(pipfd[1], STDERR_FILENO);
                close(pipfd[1]);
                char *str[4];
                str[0] = strdup("/bin/php-cgi");
                str[1] = strdup("-q");
                str[2] = strdup(req->request.uri.c_str());
                str[3] = NULL;
                if (execve(str[0], str, NULL) == -1)
                {
                    std::cout<<"execve have been failed"<<std::endl;
                    return ;
                }
            }
            close(pipfd[1]);
        }
        // else
        //     exit(0);
    }
    else
    {
        response << std::hex << bytesRead << "\r\n";
        if (bytesRead)
            response.write(buffer, bytesRead);
        else
        {
            endOfResp = 1;
            close(pipfd[0]);
        }
    }
}

std::string     Response::read_from_a_file()
{
    std::stringstream response;

    if (cgi_on == true)
    {
        exute_cgi(response);
        return response.str();
    }
    const int chunkSize = 1024;
    char buffer[chunkSize];
    memset(buffer, 0, chunkSize);

    if (!fl.is_open())
        fl.open(req->request.uri.c_str(), std::ios::binary);
    if (fl.is_open())
    {
        fl.read(buffer, chunkSize - 1);
        if (fl.gcount() > 0)
            response.write(buffer, fl.gcount());
        else
        {
            response << "\r\n";
            endOfResp = 1;
            fl.close();
        }
    }
    else
    {
        std::cerr<<"not found"<<std::endl;
        endOfResp = 1;
    }
    return response.str();
}

std::string Response::get_file_ext(std::string path)
{
    if (folder)
        return cType["html"];
    size_t pos = path.rfind(".");

    if (pos == std::string::npos)
        return cType["txt"];
    return cType[path.substr(pos + 1, path.size())];
}

bool    Response::is_cgi()
{
    std::string path = req->myServ.locations[req->loc_idx].root + req->myServ.locations[req->loc_idx].CGI_path;

    if (req->request.uri.find(path) == 0 && req->loc_idx >= 0)
        return true;
    return false;
}

std::string Response::getHdResp()
{
    struct stat statbuf;
    std::stringstream response;
    // std::cerr << "===================> " << req->myServ.locations[req->loc_idx].redirection << std::endl;
    if (req->request.status == 301)
    {
        response << "HTTP/1.1 " << req->request.status << " Moved Permanently\r\n";
        response << "Location: " << req->myServ.locations[req->loc_idx].redirection;
        endOfResp = 1;
        return response.str();
    }
    stat( req->request.uri.c_str(), &statbuf );
    if ( statbuf.st_mode & S_IRUSR )
    {
        if (statbuf.st_mode & S_IFDIR)
            folder = true;
        else if( statbuf.st_mode & S_IFREG )
            folder = false;
    }
    else
    {
        req->uri_depon_cs( 403 );
        stat( req->request.uri.c_str(), &statbuf );
    }
    response << "HTTP/1.1 ";
    response << req->request.status;
    // std::cerr<<"******************"<<req->request.status<<"**********************"<<std::endl;
    response << " OK\r\n";
    // std::cerr<<"***************************"<<std::endl;
    // if (req->request.method == "DELETE")
    // {
    //     response << "<html><body><h1>File \""<< req->request.uri <<"\" deleted.</h1></body></html>";
    //     endOfResp = 1;
    //     return response.str();
    // }
    response << "Content-Type: ";
    response << get_file_ext(req->request.uri) << "\r\n";
    // response << "text/html" << "\r\n";
    if (is_cgi() && folder == false)
    {
        response << "Transfer-Encoding: chunked";
        cgi_on = true;
    }
    else
    {
        cgi_on = false;
        response << "Content-Lenght: " << statbuf.st_size;
    }
    response << "\r\n\r\n";
    
    return response.str();
}

void    Response::getMethod()
{
    std::string hdRes = getHdResp();
    ssize_t bytesSent = send( cliSock, hdRes.c_str(), hdRes.size(), 0);
    if ( (int)bytesSent == -1 )
    {
        std::cout << "failed send" << std::endl;
    }
    else
    {
        std::cout << "sent : " << bytesSent << " bytes" <<std::endl;
        // endOfResp = 1;
        // std::cout << "data Sent Yes ......." << std::endl;
        // delSockFrEpoll( evs[i].data.fd );
        // delete( it->second );
        // reqMap.erase( evs[i].data.fd );
        // serv_cli.erase( evs[i].data.fd );
        // close( evs[i].data.fd );
    }
}

Response::Response( ReqHandler *_req, int _cliSock )
{
    cliSock = _cliSock;
    endOfResp = 0;
    req = _req;
    cType[""] = "text/plain";
    cType["php"] = "text/plain";
    cType["txt"] = "text/plain";
    cType["cpp"] = "text/plain";
    cType["hpp"] = "text/plain";
    cType["html"] = "text/html";
    cType["jpg"] = "image/jpeg";
    cType["jpeg"] = "image/jpeg";
    cType["png"] = "image/png";
    cType["gif"] = "image/gif";
    cType["pdf"] = "application/pdf";
    cType["doc"] = "application/msword";
    cType["xls"] = "application/vnd.ms-excel";
    cType["ppt"] = "application/vnd.ms-powerpoint";
    cType["zip"] = "application/zip";
    cType["tar"] = "application/x-tar";
    cType["gz"] = "application/gzip";
    cType["wav"] = "audio/wav";
    cType["mp3"] = "audio/mpeg";
    cType["mp4"] = "video/mp4";
    cType["avi"] = "video/x-msvideo";
    cType["json"] = "application/json";
    cType["xml"] = "application/xml";
    cType["css"] = "text/css";
    cType["js"] = "application/javascript";
    cType["bmp"] = "image/bmp";
    cType["ico"] = "image/x-icon";
    cType["svg"] = "image/svg+xml";
    cType["tiff"] = "image/tiff";
    cType["woff"] = "application/font-woff";
    cType["woff2"] = "application/font-woff2";
    cType["eot"] = "application/vnd.ms-fontobject";
    cType["o"] = "application/octet-stream";

    if ( req->request.method == "GET" || !req->request.method.size() )
        getMethod();
}

Response::~Response()
{
    std::cout << " Response destructor called" << std::endl;
}