#include "multiplexing.hpp"
#include "request.hpp"

std::string Request::read_buff(map<string, string> &m)
{
    string head;
    stringstream size;
    // //to get the size =======
    op.seekg(0, ios::end);
    streampos fileS = op.tellg();
    op.seekg(0, ios::beg);

    char buffer[1024];
    memset(buffer, 0, 1024 );
    op.read(buffer, 1024);
    //--- size of what read is reading ---
    std::streamsize bytesRead = op.gcount();
    line.append(buffer, bytesRead);
    //------------------------------------
    head += "HTTP/1.1 200 ok\r\nContent-Type: ";
    con_type = get_content_type(m);
    head += con_type;
    head += "\r\nContent-Lenght:";
    size << fileS;
    head += size.str();
    head += "\r\n\r\n";
    head += line;
    line = "";
    return (head);
}

void Request::read_for_send(map<string, string> &m)
{
    check_req = 1;
    string head = read_buff(m);
    len = head.length();

    send(event_fd, head.c_str(), len, 0);
    line = "";
}

void Request::end_of_file(epoll_event &event, int epoll_fd)
{
    fin_or_still = finish;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    close(event_fd);
    op.close();
    close_dir();
}

void Request::close_dir()
{
    if (dire)
    {
        closedir(dire);
        dire = NULL;
    }
}

int Request::check_permission(string str)
{
    if (access((str).c_str(), F_OK) != -1)
    {
        if ((access((str).c_str(), R_OK) && access((str).c_str(), W_OK) && access((str).c_str(), X_OK)) == 0)
            return (1);
        else
            return (0);
    }
    return (0);
}

void Request::delete_content(string pat, string file)
{
    DIR* FOLDER;
    struct dirent* entre;
    struct dirent* entre1;
    if (file_get == "")
    {
        FOLDER = opendir(pat.c_str());
        if (FOLDER)
        {
            while ((entre = readdir(FOLDER)) != NULL)
            {
                string name = entre->d_name;
                if (name[0] != '.')
                {
                    string str = pat + "/" + name;
                    cout << str << endl;
                    if (check_permission(str) == 1)
                    {
                        DIR* fold = opendir(str.c_str());
                        if (fold)
                        {
                            if ((entre1 = readdir(fold)) != NULL)
                                delete_content((str), "");
                            else
                                std::remove((str).c_str());
                            closedir(fold);
                            fold = NULL;
                        }
                        else
                            std::remove((str).c_str());
                    }
                    else
                        cout << "Error Page\n";
                }
            }
            close_dir();
            std::remove((pat).c_str());
        }
        else
            cout << "Error Page\n";
        closedir(FOLDER);
        FOLDER = NULL;
        (void)file;
    }
    // else
    // {
    //         cout << "frrrrrrrrrrrrrrrrr\n";
    //     dire = opendir(pat.c_str());
    //     if (dire)
    //     {
    //         if (check_permission(pat + "/" + file) == 1)
    //             std::remove((pat + "/" + file).c_str());
    //         else{}
    //             //error
    //     }
    //     close_dir();
    // }
    // if ((entre = readdir(dire)) == NULL)
    // {
    //     close_dir();

    //     std::remove((pat).c_str());
    //     // check = 1;
     
    // }
}

// void Request::delete_content(string pat, string file, string first)
// {
//     struct dirent* entre;
//     struct dirent* entre1;

//     if (file_get == "")
//     {
//         dire = opendir(pat.c_str());
//         if (dire)
//         {
//             // entre = readdir(dire);
//             // cout << "----- << " << entre->d_name << endl;
//             //             entre = readdir(dire);
//             // cout << "----- << " << entre->d_name << endl;
//             //             entre = readdir(dire);
//             // cout << "----- << " << entre->d_name << endl;
//             // if ((entre = readdir(dire)) == NULL)
//             // {
//             //     cout << "=========>> " << pat << endl;
//             //     std::remove((pat).c_str());
//             //     return ;
//             // }
//             while (dire && (entre = readdir(dire)) != NULL)
//             {
//                 cout << "3iiiiiiwwwwwwwwwwww\n" << endl;
//                 string name = entre->d_name;
                
//                 if (name[0] != '.')
//                 {
//                     string str = pat + "/" + name;
//                     if (check_permission(str) == 1)
//                     {
//                         // flag = 1;
//                         DIR* fold = opendir(str.c_str());
//                         if (fold)
//                         {
//                             // cout << "--->> " << str << endl;
//                             if ((entre1 = readdir(fold)) != NULL)
//                             {
//                                 cout << "/////////////////\n";
//                                 delete_content((str), "",first);
//                             }
//                             else
//                                 std::remove((str).c_str());
//                             closedir(fold);
//                             fold = NULL;
//                         }
//                         else
//                             std::remove((str).c_str());
//                     }
//                 // std::remove((str).c_str());
//                 }
//             }
//         }
//         close_dir();
//         (void)file;
//     }