#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
// bool isDirectory(const std::string& path) {
//     struct stat statResult;
//     if (stat(path.c_str(), &statResult) == 0) {
//         return S_ISDIR(statResult.st_mode);
//     }
//     return false;  // Error occurred or file doesn't exist
// }

// std::string decode( std::string str, std::map<std::string, std::string> &mp )
// {
//     std::string ret;
//     for ( size_t i = 0 ; i < str.size() ; i++ )
//     {
//         if ( str[i] == '%' && i + 2 < str.size() )
//         {
//             std::string key = str.substr( i, 3 );
//             std::map<std::string, std::string>::iterator it = mp.find( key );
//             if ( it != mp.end() )
//             {
//                 ret.push_back( it->second.front() );
//                 i += 2;
//             }
//             else
//                 ret.push_back( str[i] );
//         }
//         else
//             ret.push_back( str[i] );
//         // hello%45gg
//     }
//     return ret;
// }

std::vector<std::string> split_uri( std::string &str )
{
    std::vector<std::string>    ret;
    std::stringstream           ss( str );
    std::string                 elem;

    while ( std::getline( ss, elem, '/' ) )
    {
        if ( !elem.size() )
            continue;
        ret.push_back( elem );
    }
    return ret;
}


#include <limits.h>
int main() {


    std::string relativePath = "../data";

    char resolvedPath[PATH_MAX];

    if (realpath(relativePath.c_str(), resolvedPath) != nullptr) {
        std::cout << "Absolute path: " << resolvedPath << std::endl;
        std::string s = resolvedPath;
        std::vector<std::string> vc = split_uri( s );
        for ( size_t i = 0; i < vc.size() ; i++)
        {
            std::cout << vc[i] << std::endl;
        }
    } else {
        perror("Error resolving path");
    }
    char pa[PATH_MAX];
    getcwd( pa, sizeof(pa) );
    std::cout << "getcwd : " << pa << std::endl;
    return 0;
    // std::map<std::string, std::string> myMap;
    // myMap["%20"] = ' '; myMap["%21"] = '!'; myMap["%22"] = '"'; myMap["%23"] = '#';
    // myMap["%24"] = '$'; myMap["%25"] = '%'; myMap["%26"] = '&'; myMap["%27"] = "'";
    // myMap["%28"] = '('; myMap["%29"] = ')'; myMap["%2A"] = '*'; myMap["%2B"] = '+';
    // myMap["%2C"] = ','; myMap["%2D"] = '-'; myMap["%2E"] = '.'; myMap["%2F"] = '/';
    // myMap["%3A"] = ':'; myMap["%3B"] = ';'; myMap["%3C"] = '<'; myMap["%3D"] = '=';
    // myMap["%3E"] = '>'; myMap["%3F"] = '?'; myMap["%40"] = '@'; myMap["%5B"] = '[';
    // myMap["%5C"] = '\\';myMap["%5D"] = ']'; myMap["%5E"] = '^'; myMap["%5F"] = '_';
    // myMap["%60"] = '`'; myMap["%7B"] = '{'; myMap["%7C"] = '|'; myMap["%7D"] = '}';
    // myMap["%7E"] = '~';


    // std::string g = "5%^";
    // std::string gd = decode(g, myMap);
    // std::cout << gd << std::endl;
    // std::string path = "../../Desktop/gg";

    // if (isDirectory(path)) {
    //     std::cout << "It's a directory." << std::endl;
    // } else {
    //     std::cout << "It's a file." << std::endl;
    // }

    // return 0;
}
/*
std::map<std::string, std::string> mimeMap;

// Text types
mimeMap["text/html"] = ".html";
mimeMap["text/css"] = ".css";
mimeMap["text/javascript"] = ".js";
mimeMap["text/plain"] = ".txt";

// Application types
mimeMap["application/json"] = ".json";
mimeMap["application/xml"] = ".xml";
mimeMap["application/pdf"] = ".pdf";
mimeMap["application/msword"] = ".doc";
mimeMap["application/vnd.ms-excel"] = ".xls";
mimeMap["application/vnd.ms-powerpoint"] = ".ppt";
mimeMap["application/zip"] = ".zip";
mimeMap["application/x-tar"] = ".tar";
mimeMap["application/gzip"] = ".gz";

// Image types
mimeMap["image/jpeg"] = ".jpg";
mimeMap["image/png"] = ".png";
mimeMap["image/gif"] = ".gif";
mimeMap["image/bmp"] = ".bmp";

// Audio types
mimeMap["audio/wav"] = ".wav";
mimeMap["audio/mpeg"] = ".mp3";

// Video types
mimeMap["video/mp4"] = ".mp4";
mimeMap["video/x-msvideo"] = ".avi";

// Font types
mimeMap["application/font-woff"] = ".woff";
mimeMap["application/font-woff2"] = ".woff2";
mimeMap["application/vnd.ms-fontobject"] = ".eot";
mimeMap["application/x-font-ttf"] = ".ttf";

// Misc types
mimeMap["application/octet-stream"] = ""; // Default binary


*/