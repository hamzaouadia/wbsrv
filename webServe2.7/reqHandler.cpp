
#include "includes/reqHandler.hpp"
// to do 
/*
    -- redirection must recheck the path from the beginning, getFinalUri
    -- same for index ---

    




    -- do a uri decoder
    -- '%20 %20' -- a file named %20.txt
    -- check and parse path -real path
    -- getServer by servname host ...
    -- post continuation
    -- time out checker

*/

int ReqHandler::isLocation( std::string &str )
{
    size_t i = 0;
    for (  ; i < myServ.locations.size() ; i++ )
        if ( str == myServ.locations[i].name )
            return 1;
    return 0;
}

s_location  ReqHandler::getLocationByName( std::string &str )
{
    size_t i = 0;
    for (  ; i < myServ.locations.size() ; i++ )
    {
        if ( str == myServ.locations[i].name )
        {
            loc_idx = i;
            return myServ.locations[i];
        }
    }
    return myServ.locations[i];
}

std::string ReqHandler::getFullUri( std::vector<std::string> &spl_uri, std::string &root, int j )
{
    std::string ret = root;
    if ( ret.size() && ret[ret.size() - 1] == '/' )
        ret.erase(ret.size() - 1);
    for ( size_t i = j ; i < spl_uri.size() ; i++ )
    {
        ret = ret + "/";
        ret = ret + spl_uri[i];
    }
    return ret;
}

// if the request is file
// return
// method 

// if the request is folder
// method
// return
// index 
// autoIndex

// bool isDirectory(const std::string& path) {
//     struct stat statResult;
//     if (stat(path.c_str(), &statResult) == 0) {
//         return S_ISDIR(statResult.st_mode);
//     }
//     return false;  // Error occurred or file doesn't exist
// }

// std::string getExt( std::string str )
// {
//     std::vector<std::string> vec = split_uri( str );
//     std::string s = vec.back();
//     size_t p = s.rfind( '.' );
//     if ( p != std::string::npos && p < s.length() - 1 )
//         return ( s.substr( p + 1 ) );
//     return "";
// }

void    ReqHandler::checkRetnMeth( s_location &loc, std::vector<std::string> &splited_uri, int i )
{
    if ( (request.method == "GET" && loc.get == "off" )
        || (request.method == "POST" && loc.post == "off" )
        ||  (request.method == "DELETE" && loc.del == "off" ) )
        uri_depon_cs( 405 );
    else
    {
        if ( !i )
            request.uri = getFullUri( splited_uri, loc.root, 1 );
        else if ( i == 2 )
            request.uri = getFullUri( splited_uri, loc.root, 0 );
        else if ( i == 1 )
            request.uri = loc.root;
        request.status = 200;
    }
    // if ( loc.redirection.size() && request.method == "GET" ) // Method get check this
    // {
    //     request.uri = loc.redirection;
    //     request.status = 301;
    // }
}

void ReqHandler::getFinalUri( std::string str )
{
    std::vector<std::string> splited_uri = split_uri( str );
    std::cout << "uri to split >>>>>>>> " << str << std::endl;
    if ( !splited_uri.size() )
    {
        //root is the requested directory go check listing auto index ....
        std::string loc_str = "/";
        if ( isLocation( loc_str ) )
        {
            s_location loc = getLocationByName( loc_str );
            checkRetnMeth( loc, splited_uri, 1 );
        }
        else
        {
            request.uri = myServ.root;
            request.status = 200;
        }
    }
    else
    {
        std::string loc_str = "/" + splited_uri.front();
        if ( isLocation( loc_str ) )
        {
            std::string concat_uri;
            s_location  loc = getLocationByName( loc_str );
            checkRetnMeth( loc, splited_uri, 0 );
        }
        else
        {
            std::string _loc_str = "/";
            if ( isLocation( _loc_str ) )
            {
                s_location loc = getLocationByName( _loc_str );
                checkRetnMeth( loc, splited_uri, 2 );
            }
            else
            {
                request.uri = getFullUri( splited_uri, myServ.root, 0 );
                request.status = 200;
            }
        }
    }
}

// void    ReqHandler::getRequestFinalForm()
// {
//     request.method = req.front();
//     request.uri = getFinalUri();
//     std::cout << "-------- request URI -----------" <<  request.uri << std::endl;
// }

void    ReqHandler::reqStrToVec( std::string &line )
{
    std::istringstream fline( line );
    std::string word;
    while( fline >> word )
    {
        req.push_back( word );
    }
    // handle the case printf("") | telnet localhost ....
    // if ( req.size() != 3 )
    // {
    //     request.status = 400;
    //     // request.uri put the path of the 400 error
    // }
}

std::vector<std::string> lineSplit( std::string &line )
{
    std::istringstream fline( line );
    std::string word;
    std::vector<std::string> ret;
    while( fline >> word )
    {
        // std::cout << "\033[31m" << word << "\033[0m" << std::endl;
        ret.push_back( word );
    }
    return ret;
}

void ReqHandler::countBodyBytes( std::string &str )
{
    std::string sequence = "\n\r\n\r";
    size_t pos = str.find(sequence);
    int     s = 0;
    if ( pos != std::string::npos )
    {
        s = pos;
        bodyStartFound = true;
    }
    else
    {
        non_body_str = str;
        return ;
    }
    std::string res = str.substr( s, str.size() - s );
    non_body_str = str.substr( 0, str.size() - res.size() );

    std::cout << " NON body data : |" << non_body_str << "|"<< std::endl;
    std::cout << " body data : |" << res << "|"<< std::endl;
    if ( request.method == "POST" && res.size() > myServ.limit )
        return ( uri_depon_cs( 413 ) );
    body_string = res;
    bytes_red = res.size();
}

void    ReqHandler::uri_depon_cs( int code )
{
    // search in the map in server, that contains codes with their paths
    // if found ---> request.uri = map[code];
    std::map<int, std::string>::iterator it = myServ.errorpage.find( code );
    request.status = code;
    endOfRead = 1;
    if ( it != myServ.errorpage.end() )
        request.uri = it->second;
    else
        request.uri = myServ.defErrP[ code ];
}

void    ReqHandler::fillReqHeaders()
{
    for ( size_t i = 1 ; i < reqHds.size() ; i++ )
    {
        std::string first;
        std::string second;
        std::string word;
        std::istringstream src( reqHds[i] );
        src >> first;
        while ( src >> word )
        {
            second += word;
            second += " ";
        }
        if ( second.size() )
        {
            second.erase( second.end() - 1 );
            reqHeaders[first] = second;
        }
    }
    // passeOnce = 1;
    reqHds.clear();
}

int     ReqHandler::getHeaderVal( std::string key, std::string &val )
{
    std::map<std::string, std::string>::iterator it = reqHeaders.find( key );
    if ( it == reqHeaders.end() )
        return 0;
    else
        return (val = it->second, 1);
}

void    ReqHandler::parseHeaders()
{
    // content-lenght // transfer-encoding // 
    // content-type //  host 
    // std::string value;
    if ( !getHeaderVal( "Host:", value ) )
    {
        return ( uri_depon_cs( 400 ) );
    }
    if ( request.method == "POST" )
    {
        if ( !getHeaderVal( "Transfer-Encoding:", value) )
        {
            if ( !getHeaderVal( "Content-Lenght:", value) )
            {
                uri_depon_cs( 411 );
            }
            else
            {
                std::istringstream iss( value );
                iss >> content_lenght;
                // here value has the content lenght
            }
        }
        else
        {
            if ( value != "chunked" )
                uri_depon_cs( 501 );
                // here value has the transfert encoding
        }
    }
    std::cout << "parse Headers here" << std::endl;
}

std::string decodeUri( std::string str )
{
    std::map<std::string, std::string> mp;
    mp["%20"] = ' '; mp["%21"] = '!'; mp["%22"] = '"'; mp["%23"] = '#';
    mp["%24"] = '$'; mp["%25"] = '%'; mp["%26"] = '&'; mp["%27"] = "'";
    mp["%28"] = '('; mp["%29"] = ')'; mp["%2A"] = '*'; mp["%2B"] = '+';
    mp["%2C"] = ','; mp["%2D"] = '-'; mp["%2E"] = '.'; mp["%2F"] = '/';
    mp["%3A"] = ':'; mp["%3B"] = ';'; mp["%3C"] = '<'; mp["%3D"] = '=';
    mp["%3E"] = '>'; mp["%3F"] = '?'; mp["%40"] = '@'; mp["%5B"] = '[';
    mp["%5C"] = '\\';mp["%5D"] = ']'; mp["%5E"] = '^'; mp["%5F"] = '_';
    mp["%60"] = '`'; mp["%7B"] = '{'; mp["%7C"] = '|'; mp["%7D"] = '}';
    mp["%7E"] = '~';
    std::string ret;
    for ( size_t i = 0 ; i < str.size() ; i++ )
    {
        if ( str[i] == '%' && i + 2 < str.size() )
        {
            std::string key = str.substr( i, 3 );
            std::map<std::string, std::string>::iterator it = mp.find( key );
            if ( it != mp.end() )
            {
                ret.push_back( it->second[0] );
                i += 2;
            }
            else
                ret.push_back( str[i] );
        }
        else
            ret.push_back( str[i] );
    }
    return ret;
}

int find_me( char c )
{
    std::string s = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    for( size_t i = 0 ; i < s.size() ; i++ )
        if ( c == s[i] )
            return 1;
    return 0;
}

int checkUri( std::string &str )
{
    str = decodeUri( str );
    for( size_t i = 0 ; i < str.size() ; i++ )
        if( !find_me( str[i] ) )
            return 0;
    return 1;
}

void    ReqHandler::fileOrDir()
{
    // std::cerr << "fileOrDir" << std::endl;
    if ( loc_idx != -1 && myServ.locations[loc_idx].redirection.size() && request.method == "GET" ) // Method get check this
    {
        // std::cerr << "got inside" << std::endl;
        request.uri = myServ.locations[loc_idx].redirection;
        request.status = 301;
    }
    else if ( isDirectory( request.uri ) )
    {
        if ( request.method == "GET" && loc_idx != -1 )
        {
            if ( !myServ.locations[loc_idx].index.size() )
            {
                std::cout << "HERE locIDX : " << loc_idx << std::endl; 
                if ( myServ.locations[loc_idx].autoidx == "on" )
                    request.status = 200;
                else
                    uri_depon_cs( 403 );
            }
            else
            {
                if ( myServ.locations[loc_idx].root[myServ.locations[loc_idx].root.size() - 1] == '/' )
                    myServ.locations[loc_idx].root.erase( myServ.locations[loc_idx].root.size() - 1 );
                request.uri = myServ.locations[loc_idx].root + myServ.locations[loc_idx].index;
                request.status = 200;
            }
        }
    }
    // if ( access(request.uri.c_str(), F_OK) )
    //     return ( uri_depon_cs( 404 ) );
    // {
    // else
    // }
    
    // if ( loc_idx != -1 && myServ.locations[loc_idx].redirection.size() && request.method == "GET" ) // Method get check this
    // {
    //     std::cout << "location name --> " << myServ.locations[loc_idx].name << std::endl;
    //     // redirection is a path- it must be sent to get uri again
    //     // getFinalUri( myServ.locations[loc_idx].redirection );
    //     request.uri = myServ.locations[loc_idx].redirection;
    //     request.status = 301;
    // }
    // else if ( isDirectory( request.uri ) )
    // {// file
    //     if ( request.method == "GET" && loc_idx != -1 )
    //     {
    //         if ( !myServ.locations[loc_idx].index.size() )
    //         {
    //             std::cout << "HERE locIDX : " << loc_idx << std::endl;
    //             if ( myServ.locations[loc_idx].autoidx == "on" )
    //                 request.status = 200;
    //             else
    //                 uri_depon_cs( 403 );
    //         }
    //         else
    //         {
    //             if ( myServ.locations[loc_idx].root[myServ.locations[loc_idx].root.size() - 1] == '/' )
    //                 myServ.locations[loc_idx].root.erase( myServ.locations[loc_idx].root.size() - 1 );
    //             request.uri = myServ.locations[loc_idx].root + myServ.locations[loc_idx].index;
    //             request.status = 200;
    //         }
    //     }
    // }
    // if ( access(request.uri.c_str(), F_OK) )
    //     return ( uri_depon_cs( 404 ) );
}

// Serv    ReqHandler::getServer()
// {
//     std::vector<Serv> srvs;
//     for( size_t i = 0 ; i < servers.size() ; i++ )
//     {
//         if ( socknData[fdServ].sin_port == htons( servers[i].port ) )
//             srvs.push_back( servers[i] );
//     }
//     if( srvs.size() == 1 )
//         return srvs.front();
    
// }

void    ReqHandler::parse_request()
{
    // if ( !passeOnce )
    // {
    std::cout << "REQUEST PARSING -------------" << std::endl;
    // std::cerr << "--------------"<<non_body_str<<"-------------" << std::endl;
    if ( !reqHds.size() )
        return( uri_depon_cs( 400 ) );
    reqStrToVec( reqHds.front() );
    if ( req.size() != 3 )
        return( uri_depon_cs( 400 ) );
    else if ( req.front() != "GET" &&  req.front() != "POST" &&  req.front() != "DELETE" )
        return( uri_depon_cs( 501 ) );
    else if ( !checkUri( req[1] ) ) // need to decode URI *****************************************************
        return ( uri_depon_cs( 400 ) );
    else if ( req.back() != "HTTP/1.1" )
        return( uri_depon_cs( 505 ) );
    // else if ( req[1].size() > 500 ) // not necessary
    //     return( uri_depon_cs( 414 ) );

    fillReqHeaders();
    
    // myServ = getServer();
    // get the server depending on headers and port number

    request.method = req.front();
    getFinalUri( req[1] );
    std::cout << "\033[31m============================" << "concat uri : " << request.uri << "\033[0m" << std::endl;
    fileOrDir();  
    parseHeaders();
    if ( request.method != "POST" )
        endOfRead = 1;
    else
    {

    }
    // }
    
    std::cout << "-------- request URI -----------" <<  request.uri << std::endl;
    // fillReqHeaders( 0 );
}

void    ReqHandler::nextBuff( char *buff )
{
    std::string myData( buff );
    if ( request.method == "POST" )
    {
        if ( bodyStartFound )
        {
            body_string = myData; // open file - write in file - file must be in the object
            bytes_red += body_string.size();
            if ( bytes_red > myServ.limit )
                return ( uri_depon_cs( 413 ) );
        }
        else
            countBodyBytes( myData );
    }
    else
    {

    }
}

void    ReqHandler::checkBuff( char *buff, size_t bytes )
{
    std::string myData( buff );
    passedOnce = true;
    std::cout << bytes << " bytes popopo" << std::endl;
    // if ( bytes < 1023 )
    //     endOfRead = 1;
    std::cout << "in check buffer" << std::endl;
    std::cout << " >> -*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* << " << std::endl;
    std::cout << "\033[31m" << myData << "\033[0m" << std::endl;
    std::cout << " >> -*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* << " << std::endl;
    countBodyBytes( myData );
    std::istringstream src( non_body_str );
    std::string line;
    while ( std::getline( src, line ) )
        reqHds.push_back( line );
    parse_request();
}

// ReqHandler::ReqHandler( std::map<int, sockaddr_in> &_socknData, int _fdServ, std::vector<Serv> &_servers )
ReqHandler::ReqHandler( Serv &_myServ )
{
    passedOnce = false;
    // passeOnce = 0;
    // test above
    // socknData = _socknData;
    // fdServ = _fdServ;
    // servers = _servers;
    myServ  = _myServ;
    loc_idx = -1;
    bytes_red = 0;
    content_lenght = -1;
    bodyStartFound = false;
    endOfRead = 0;
    std::cout << "---- IN reqhandler CONSTRUCTOR ----" << std::endl;
}


// ep->events[i].events & EPOLLERR || ep->events[i].events & EPOLLHUP || ep->events[i].events & EPOLLRDHUP


// /path/hello/
// /path/hello//hello      -- slash clean
// /hello/file.txt
// /path/hello/location/