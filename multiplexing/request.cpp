#include "request.hpp"
Request::Request(){
    // cout<< "*****" <<this->_port << endl;
}
Request::Request(map<int, pair<string, string> > server_book, int fd_client)
{
(void)server_book;
(void)fd_client;
    get_to_cgi = false;
    size_read = 0;
    size_read_request = 0;
    size_request = 0;
    check_left_header = 0;
    check_create_file = 0;
    fin_or_still = Still;
    check_req = 0;
    len = 0;
    fir_body = "NULL";
    dire = NULL;
    exten = "";
    read_request = "";
    ck = 0;
    event_fd = 0;
        finir = 0;
      size = 0;
      total = 0;
      next_chunked = 0;
      acces_read_in_post = 0;
      err = 0;
      last = 0;
       fake_bondary = "NULL";
      wait = 0;
      fill_content_type();
}

Request::Request(const Request& obj)
{
     fake_bondary = "NULL";
      wait = 0;
      last = 0;
    get_to_cgi = obj.get_to_cgi;
    event_fd = 0;
    ck = 0;
    read_request = "";
    exten = "";
    size_read = 0;
    size_read_request = 0;
    size_request = 0;
    check_left_header = 0;
    check_create_file = 0;
    // check_first_line = 0;
    kk = 0;
    len = 0;
    dire = NULL;
    err = 0;
    fin_or_still = Still;
    // check_first_time = 0;
    // counter = obj.counter;
    check_req = 0;
    fir_body = "NULL";
    finir = 0;
      size = 0;
      total = 0;
      next_chunked = 0;
      acces_read_in_post = 0;
      fill_content_type();
      (void) obj;
}
 


int Request::parse_line(std::string line, int check_first)
{
    if (check_first == 0)
    {
        size_t found_POST = line.find("POST");
        size_t found_GET = line.find("GET");
        size_t found_DELETE = line.find("DELETE");
        if (found_POST != std::string::npos)
        {
            header_request.insert(std::make_pair(line.substr(0,found_POST+4),   (line.substr(found_POST + 5,line.length()).c_str())));
            this->methode = POST;
            // cout << "00000000000000000000000000\n";
        }
        else if (found_GET != std::string::npos)
        {
            header_request.insert(std::make_pair(line.substr(0,found_GET+3),   (line.substr(found_GET + 4,line.length()).c_str())));
            this->methode = GET;
            // cout << "111111111111111111111111\n";
        }
        else if (found_DELETE != std::string::npos)
        {
            header_request.insert(std::make_pair(line.substr(0,found_DELETE+6),   (line.substr(found_DELETE + 7,line.length()).c_str())));
            this->methode = DELETE;
        }
        else
        {
            this->methode = NONE;
        }
        
    }
    else
    {
        size_t found_POINT = line.find(":");
        // cout << found_POINT << " " << line.length() << " ---\n";
        header_request.insert(std::make_pair(line.substr(0,found_POINT),   (line.substr(found_POINT + 2,line.length() - 1).c_str())));
    }
    return 0;
}

int Request::parce_request(string read_request, epoll_event &event, int epoll_fd, servers &config)
{
    (void)event;
    (void)epoll_fd;
    (void)config;

    size_t i = 0;
    if (check_left_header == 1)
    {
        return 1;
    }
    size_t   check_body = read_request.find("\r\n\r\n");
    if (check_body != std::string::npos)
    {
        check_left_header = 1;
    }
        
    check_body++;
    std::string line;
    int check_first_line = 0;
    while (read_request[i] )//&& i <= check_body
    {

        if (read_request[i] != '\n')
            line += read_request[i];
        else
        {
            parse_line(line, check_first_line);
            line = "";
            check_first_line = 1;
        }
        i++;
    }
    // cout << line << endl;
    //     exit(1);
    std::string l = header_request["Content-Length"];
		size_request = atol(l.c_str());
        if (check_body != std::string::npos && methode == "POST")
        {
            this->fir_body = read_request.substr(check_body + 3, read_request.length());
        }
     
    
    return 0;
 
}
 

void Request::fill_content_type()
{
    cont_type["text/plain\r"] = ".txt";
    cont_type["image/gif\r"] = ".gif";
    cont_type["text/html\r"] = ".html";
    cont_type["image/jpeg\r"] = ".jpeg";
    cont_type["image/png\r"] = ".png";
    cont_type["application/json\r"] = ".json";
    cont_type["text/javascript\r"] = ".js";
    cont_type["audio/mpeg\r"] = ".mp3";
    cont_type["video/mp4\r"] = ".mp4";
    cont_type["video/mpeg\r"] = ".mpeg";
    cont_type["application/pdf\r"] = ".pdf";
    cont_type["video/webm\r"] = ".webm";
    cont_type["application/zip\r"] = ".zip";
}


void Request::create_file(std::ofstream& outputFile,   std::map<std::string, std::string>& map, servers &config, int index)
{
 
   std::string type_file = map["Content-Type"];
   
     std::map<std::string, std::string>::iterator it = header_request.find(type_file);
    srand (time(NULL));
    std::ostringstream str;
    str << rand();

    if (it != cont_type.end())
    {
        std::string randomName =     config[index].get_loc_up_folder(Path) + "/" + str.str() + cont_type[type_file];
        outputFile.open(randomName.c_str());

    }  
    else  
    {
        std::string randomName = str.str() + ".x";
        outputFile.open(randomName.c_str());
    }
}


void Request::binary(servers &config, int index)
{
    if (check_create_file == 0)
	{
		create_file(outputFile, header_request, config,   index);
		check_create_file = 1;
		if (fir_body != "NULL")
		{
			outputFile << fir_body;
			fir_body = "NULL";
		}
		else
			outputFile << read_request;
	}
	else
	{
		outputFile << read_request;			
	}
}


void Request::chunked(servers &config, int index)
{
           

 
    if (check_create_file == 0)
	{
		create_file(outputFile, header_request, config,  index);
		check_create_file = 1;

        if (fir_body != "NULL")
		{
            int position_int = fir_body.find("\r\n"); 
            std::string num = fir_body.substr(0, position_int);
              char *endptr;
             
            size_chunked =  strtol(num.c_str(), &endptr, 16);
        
            fir_body = fir_body.substr(position_int + 2, fir_body.length());
            size_t check_last = fir_body.find("\r\n0\r\n");
            if (check_last != string::npos)
                fir_body = fir_body.substr(0, check_last);
			  outputFile << fir_body;
                // total += (size - position_int + 2);
             size_chunked -= fir_body.length();
			fir_body = "NULL";
		}
		else
		{
			  outputFile << read_request;
        //    read_request.erase();
		}
	}
	else
	{
		outputFile << read_request;			
	}
}

void Request::create_file_bondar(std::ofstream& outputFile,   std::map<std::string, std::string>& map, servers &config, int index, string name, string content)
{
 (void)map;
 content = content+ "\r";
    cout << content<<endl;

    //  outputFile.open("tt.txt");
    // return;
 
   std::string type_file = content;
    
     std::map<std::string, std::string>::iterator it = cont_type.find(type_file);
    srand (time(NULL));
    std::ostringstream str;
    str << rand();

    if (it != cont_type.end())
    {
        std::string randomName =     config[index].get_loc_up_folder(Path) + "/" + name   + cont_type[content];
        cout << randomName << endl;
        outputFile.open(randomName.c_str());

    }  
    else  
    {
        std::string randomName = str.str() + ".x";
        outputFile.open(randomName.c_str());
    }

//  exit(1);
}

void Request::boundar(servers &config, int index)
{
           
    if (check_create_file == 0)
	{
        string content = header_request["Content-Type"];
     
        boudri = content.substr(content.find("=") + 1, content.length() -(content.find("=") + 1) - 1);
        last_boundri = boudri;
        last_boundri += "--";
        // cout << content << endl;
        // cout << last_boundri << endl;
        //  exit(1);
		check_create_file = 1;

        if (fir_body != "NULL")
		{
            int position_int = fir_body.find("\r\n\r\n"); 
            std::string boundary = fir_body.substr(0, position_int);
            string filename_content = boundary.substr(boundary.find("name=\""));
            string filename = filename_content.substr(6, filename_content.find("\";")  - 6);

            string Content = filename_content.substr(filename_content.find("\r\n") + 16);
		    create_file_bondar(outputFile, header_request, config,  index, filename, Content);
            fir_body = fir_body.substr(position_int + 4 , fir_body.length());
            //  cout << fir_body;
            // exit(1);
			  outputFile << fir_body;

             size_chunked -= fir_body.length();
			fir_body = "NULL";
		}
		else
		{
			  outputFile << read_request;
        //    read_request.erase();
		}
	}
	else
	{
		outputFile << read_request;			
	}
}



void Request::boundaries(servers &config, int index, int fd)
{
    char buff[1024];
    memset(buff, 0, 1024);
    if (acces_read_in_post == 1)
    {
        size_t a = read(fd, buff, 1024);
        read_request.clear();
        if (fake_bondary != "NULL")
        {
            read_request = fake_bondary;
            fake_bondary = "NULL";
        }
        read_request.append(buff,a);
        //    outputFile << read_request;

        size_t found_bondary = read_request.find(boudri);
        if (found_bondary == string::npos)
        {
            fake_bondary = read_request.substr(read_request.length() - boudri.length()); // , read_request.length()
            read_request = read_request.substr(0, read_request.length() - boudri.length());
            outputFile << read_request;
        }
        else
        {
            size_t last_bondary = read_request.find(last_boundri);
            if (last_bondary != string::npos )//&& a < 1024
            {
                // if (boundary.find("name=\"") != string::npos)
                read_request = read_request.substr(0, last_bondary - 4);
                outputFile << read_request ;
                size_t found_bondary_2 = read_request.find(boudri);
                if (found_bondary_2 != string::npos) // mochkil kayn kay kon file hna
                {
                    size_t position_int = read_request.find("\r\n\r\n");
                    if ((position_int + 4) < read_request.length() && last != 1)
                    {
                        cout << read_request << endl << "----------------------------\n";
                        fake_bondary = read_request.substr(position_int + 4, read_request.length());  
                        cout << fake_bondary << endl;
                        // outputFile << read_request ;

                    }
                     std::string boundary = read_request.substr(found_bondary, position_int);
                string filename_content = boundary.substr(boundary.find("name=\""));
                string filename = filename_content.substr(6, filename_content.find("\";")  - 6);
                string Content = filename_content.substr(filename_content.find("\r\n") + 16, filename_content.find("\r\n\r\n") - (filename_content.find("\r\n") + 16) ) ;
                read_request = read_request.substr(0, found_bondary -2 );
                // // cout << read_request;
                // // exit(1);
                outputFile << read_request ;
                 outputFile.close();
		        create_file_bondar(outputFile, header_request, config,  index, filename, Content);
                outputFile << fake_bondary;
                    // cout << read_request;
                }
                last = 1;
                finir = 1;
                 return;
            }
       
                fake_bondary = "NULL";
                size_t position_int = read_request.find("\r\n\r\n");
                if ((position_int + 4) < read_request.length() && last != 1)
                    fake_bondary = read_request.substr(position_int + 4, read_request.length());              
                std::string boundary = read_request.substr(found_bondary, position_int);
                string filename_content = boundary.substr(boundary.find("name=\""));
                string filename = filename_content.substr(6, filename_content.find("\";")  - 6);
                string Content = filename_content.substr(filename_content.find("\r\n") + 16, filename_content.find("\r\n\r\n") - (filename_content.find("\r\n") + 16) ) ;
                 outputFile.close();
                read_request = read_request.substr(0, found_bondary -2 );
                outputFile << read_request ;
		        create_file_bondar(outputFile, header_request, config,  index, filename, Content);

                read_request.clear();
        }
       
        return;
        // exit(1);
    }
    if (check_left_header == 1)
            acces_read_in_post = 1;
    boundar(config,   index);
}

void Request::post(int fd, servers &config, epoll_event &event)
{
     std::map<std::string, std::string>::iterator it = header_request.find("Transfer-Encoding");
   
    //  (void)fd;
            // map<string, string>::iterator it1;
            map<string, string>::iterator it0;
            // it1 = header_request.find("Transfer-Encoding");//Content-Length
            it0 = header_request.find("Content-Length");//Content-Length
            if ((it == header_request.end()) && (it0->second.empty()))
            {
                error_page(event, fd, "400", config);
             
                }
    this->parse_url_prot("POST", fd, event, config);
    int index = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));
    // exit(4);
    // if ((this->Path_bef[Path_bef.length() - 1] != '/'))
    // {
    //             redirection_content_backSlash(event, fd, config);
    //     err = 1;

    //     return;
    // }
    if ((config[index].get_loc_post(this->Path) == 0) && (Path.find(".") == string::npos))
    {
        err = 1;
        error_page(event, fd, "405", config);
        return;
    }
    if ( config[index].get_loc_upload(this->Path) == 0)
    {
        err = 1;
        error_page(event, fd, "403", config);
        return;
    }

     if (it != header_request.end() && header_request["Transfer-Encoding"] == "chunked\r")
     {
         char buff[1024];
        memset(buff, 0, 1024);
        if (acces_read_in_post == 1)
        {
            if (next_chunked == 1)
            {
                int a = 0;
                a = read(fd, buff, 1024 - size);
                if (a < 1024 - size)
                {
                    size += a;
                    // total+= size;
                    read_request.append(buff,a);

                    if ( size_chunked == total)
                   {
                            string read_request2;
                        if (read_request[0] == '\r')
                        {
                            read_request2 = read_request.substr(2,size);
                        }
                        else
                        {
                            read_request2 = read_request.substr(0,size);
    
                        }

                        size_t position_int = read_request2.find("\r\n"); 
                        if (position_int == string::npos)
                            return;
                        std::string num = read_request2.substr(0, position_int);
                        char *endptr;
                        size_chunked =  strtol(num.c_str(), &endptr, 16);
                        // cout << size_chunked << " -- " << total << endl;

                        if (size_chunked == 0)
                        {
                        
                            finir = 1;
                            return;
                        }
                   }
                }
               else
               {
                    size = a;
                    read_request.append(buff,a);
                    string read_request2;
                    if (read_request[0] == '\r')
                    {
                        read_request2 = read_request.substr(2,size);
                    }
                    else
                    {
                        read_request2 = read_request.substr(0,size);
 
                    }

                    int position_int = read_request2.find("\r\n"); 
                    std::string num = read_request2.substr(0, position_int);
                    char *endptr;
                    size_chunked =  strtol(num.c_str(), &endptr, 16);
                    // cout << size_chunked << " -- " << total << endl;

                    if (size_chunked == 0)
                    {
                       
                        // cout << size << "--"<< endl;
                        finir = 1;
                        return;
                    }
                    read_request2 = read_request2.substr(position_int + 2, read_request2.length());
                    outputFile << read_request2;
                    total = read_request2.length();
                    next_chunked = 0;
                    // total = 0;
                    read_request.clear();
               }
 
            }
            
     
        else
        {
            

         
            if (size_chunked - total >= 1024) 
            {
                size = read(fd, buff, 1024);
                total+=size;
                read_request.append(buff,size);
                outputFile << read_request;	
                read_request.clear();
                size = 0;
                return;
                // size_chunked -= size;
            }
            else if (size_chunked - total > 0)
            {
                // cout << size_chunked - total << " //// \n";
                size = read(fd, buff, size_chunked - total);
                total+=size;
                if (size ==  (size_chunked - total))
                {
                    read_request.append(buff,size);
                    outputFile << read_request;
                    size = 0;
                    read_request.clear();
                    // total = 0;
                    next_chunked = 1;

                    return;
                }
                else
                {
                    read_request.append(buff,size);
                    outputFile << read_request;	
                    read_request.clear();
                    size = 0;

                    return;
                }
                
            }
            else
            {
                 next_chunked = 1;
                return;

            }
            size = 0;
            if (total >= size_chunked)
            {
                next_chunked = 1;
                return;
            }
        return;
     }
    }
        chunked(config, index);
        if (check_left_header == 1)
            acces_read_in_post = 1;
}
 
     

     else
     {

        string content = header_request["Content-Type"];
     
        // boudri = content.substr(content.find("=") + 1, content.length() -(content.find("=") + 1) - 2);
        // last_boundri = boudri;
        // last_boundri += "--";
        // cout << content << endl;
        // exit(1);
        size_t si = content.find("multipart/form-data");
        if (si != string::npos)
        {
             cout << "boundaries\n";
            boundaries(config, index, fd);
            // exit(1);
        }
        else
        {
            cout << "ana binary\n" ;
            if (!(event.events & EPOLLIN))
            {
                size_read_request = size_request;
                return;
            }
            if (acces_read_in_post == 1  && event.events & EPOLLIN)
            {

                char buff[1024];
            
                size = 0;
                size = read(fd, buff, 1024);
                size_read_request += size;
                read_request.append(buff,size);
                outputFile << read_request;
                return;
                            // parce_request(read_request);
            }
            binary(config, index);
            if (check_left_header == 1)
                acces_read_in_post = 1;
        }
     }
     
}

