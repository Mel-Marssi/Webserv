// #include "request.hpp"


// int Request::parse_line(std::string line, int check_first)
// {
//     if (check_first == 0)
//     {
//         size_t found_POST = line.find("POST");
//         size_t found_GET = line.find("GET");
//         size_t found_DELETE = line.find("DELETE");
//         if (found_POST != std::string::npos)
//         {
//             header_request.insert(std::make_pair(line.substr(0,found_POST+4),   (line.substr(found_POST + 5,line.length()).c_str())));
//             this->methode = POST;
//         }
//         else if (found_GET != std::string::npos)
//         {
//             header_request.insert(std::make_pair(line.substr(0,found_GET+3),   (line.substr(found_GET + 4,line.length()).c_str())));
//             this->methode = GET;
//         }
//         else if (found_DELETE != std::string::npos)
//         {
//             header_request.insert(std::make_pair(line.substr(0,found_DELETE+6),   (line.substr(found_DELETE + 7,line.length()).c_str())));
//             this->methode = DELETE;
//         }
//         else
//         {
//             this->methode = NONE;
//         }
//         if (this->methode != NONE)
//         {
//             std::cout << "Bad request\n";   
//         }
//     }
//     else
//     {
//         size_t found_POINT = line.find(":");
//         header_request.insert(std::make_pair(line.substr(0,found_POINT),   (line.substr(found_POINT + 2,line.length()).c_str())));
//     }
 
//     return 0;
// }

// int Request::parce_request(std::string &read_request)
// {
//     size_t i = 0;
  
//     size_t   check_body = read_request.find("\r\n\r\n");
//     if (check_body == std::string::npos)
//     {
//         check_left_header = 1;
//         return 1;
//     }
        
//     check_body++;
//     std::string line;
//     int check_first_line = 0;
//     while (read_request[i] && i <= check_body)
//     {
//         if (read_request[i] != '\n')
//             line += read_request[i];
//         else
//         {
//             parse_line(line, check_first_line);
//             line = "";
//             check_first_line = 1;
//         }
//         i++;
//     }
//     this->fir_body = read_request.substr(check_body + 3, read_request.length());
//     return 0;
//     // if (this->methode == "GET" || this->methode == "POST" || this->methode == "GET")
// }