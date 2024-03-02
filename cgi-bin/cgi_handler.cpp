#include "cgi_handler.hpp"
// 415 unseportted file
// get normal file
int cgi_handler::i = 0;

string get_cgi_path(string extention, map<string, string> cgi_exec_path)
{
	return (cgi_exec_path[extention]);
}

char **set_env_cgi(map<string, string> &CGI_BOOK)
{
	char **env = new char *[CGI_BOOK.size() + 1];
	map<string, string>::iterator it = CGI_BOOK.begin();
	for (int i = 0; it != CGI_BOOK.end(); it++, i++)
	{
		string tmp = it->first + "=" + it->second;
		env[i] = new char[tmp.size() + 1]();
		strcpy(env[i], tmp.c_str());
	}
	env[CGI_BOOK.size()] = NULL;
	return env;
}
void  execute_cgi(Request &req, server_config &config)
{
	cgi_handler cgi(req);
	string file, file_input, tmp;
	req.flag_read_cgi = 1;
	file = req.full_Path;
	try
	{
		if (req.methode == "GET")
		{
			file_input = file;
			tmp = get_cgi_path(req.file_get.substr(req.file_get.find(".")), config.get_loc_cgi_exec_path(req.Path));
			if (tmp == "")
			{
				req.flag_read_cgi = 0;
				return;
			}
		}
		else if (req.methode == "POST")
		{
			file_input = req.path_post;
			tmp = get_cgi_path(file.substr(req.full_Path.find_last_of(".")), config.get_loc_cgi_exec_path(req.Path));
			if (tmp == "")
			{

				req.flag_read_cgi = 0;
				req.cgi_file = req.path_post;
				return;
			}
		}
	}
	catch (...)
	{
		req.status_pro = "500";
		return;
	}

	time_t t = time(NULL);
	stringstream to_s;
	char **env = set_env_cgi(cgi.CGI_BOOK);
	int fd[2], fd_error;

	to_s << t;

	string tmp_file = "/tmp/_." + to_s.str();
	req.cgi_file = tmp_file;
	req.size_cgi = atol(cgi.CGI_BOOK["CONTENT_LENGTH"].c_str());

	char *argv[3] = {(char *)tmp.c_str(), (char *)file.c_str(), NULL};
	file_input.insert(0, "./");
	fd[0] =  open(file_input.c_str(), O_RDONLY);
	if (fd[0] == -1)
	{
		req.status_pro = "500";
		return;
	}
	fd[1] = open(tmp_file.c_str(), O_CREAT | O_RDWR, 0777);
	fd_error = open("./errors/error.log", O_CREAT | O_RDWR, 0777);
	gettimeofday(&req.start_cgi, NULL);
	req.pid = fork();
	if (req.pid == -1)
	{
		close(fd[0]);
		close(fd[1]);
		close(fd_error);
		req.status_pro = "500";
		for (int i = 0; env[i]; i++)
			delete[] env[i];
		return;
	}
	if (req.pid == 0)
	{

		dup2(fd[0], 0);
		dup2(fd[1], 1);
		dup2(fd_error, 2);
		close(fd[0]);
		close(fd[1]);
		close(fd_error);
		execve(argv[0], argv, env);
		for (int i = 0; env[i]; i++)
			delete[] env[i];
		delete[] env;
		return;
	}

	close(fd[0]);
	close(fd[1]);
	close(fd_error);
	for (int i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
}

cgi_handler::cgi_handler(Request &req)
{
	CGI_BOOK["SCRIPT_FILENAME"] = "." + req.Path + "/" + req.file_get;
	CGI_BOOK["REQUEST_METHOD"] = req.methode;
	CGI_BOOK["QUERY_STRING"] = req.Query_String;
	CGI_BOOK["CONTENT_LENGTH"] = req.header_request["Content-Length"];
	CGI_BOOK["CONTENT_TYPE"] = req.header_request["Content-Type"];
	CGI_BOOK["SERVER_PROTOCOL"] = "HTTP/1.0";
	CGI_BOOK["REDIRECT_STATUS"] = "200";
	CGI_BOOK["FCGI_ROLE"] = "RESPONDER";
	CGI_BOOK["REQUEST_SCHEME"] = "http";
	CGI_BOOK["GATEWAY_INTERFACE"] = "CGI/1.0";
	CGI_BOOK["SERVER_SOFTWARE"] = "webserv";
	CGI_BOOK["REQUEST_URI"] = req.Path + "/" + req.file_get;
	if (!req.header_request["Cookie"].empty())
	{
		CGI_BOOK["HTTP_COOKIE"] = req.header_request["Cookie"];
	}
}

cgi_handler::~cgi_handler()
{
}
