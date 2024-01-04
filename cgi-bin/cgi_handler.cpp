#include "cgi_handler.hpp"
#include <fcntl.h>
#include <sys/wait.h>

int cgi_handler::i = 0;
void php_cgi(Request &req, server_config &config)
{
	(void)config;
	cgi_handler cgi(req);
	char *env[cgi.CGI_BOOK.size() + 1];
	string file = req.full_Path;
	
	stringstream to_s;
	to_s << cgi_handler::i;

	cgi_handler::i++;
	string tmp_file = "/tmp/_" + to_s.str() + ".html";
	req.cgi_file = tmp_file;
	int fd[2];

	map<string, string>::iterator it = cgi.CGI_BOOK.begin();
	for(int i= 0; it != cgi.CGI_BOOK.end(); it++, i++)
	{
		string tmp = it->first + "=" + it->second;
		env[i] = (char*)tmp.c_str();
		cout << "env : " <<  env[i] << endl;
	}
	env[cgi.CGI_BOOK.size()] = NULL;

	string tmp = config.get_loc_cgi_path("/cgi-bin");
	char *argv[] = {(char *)tmp.c_str(), (char*)file.c_str(), NULL};

	if (fork() == 0)
	{
		fd[0] = open(file.c_str(), O_RDONLY);
		fd[1] = open(tmp_file.c_str(),  O_CREAT | O_RDWR | O_TRUNC, 0777);
		dup2(fd[0], 0);
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		// execve(tmp.c_str(), argv, env);
		execve(argv[0], argv, env);
		cout << "error execve" << endl;
		exit(1);
	}
cout << "waitpid" << endl;
	waitpid(-1, NULL, 0);
}
cgi_handler::cgi_handler(Request &req)
{
	CGI_BOOK["SCRIPT_FILENAME"] = req.Path+"/"+req.file_get;
	CGI_BOOK["REQUEST_METHOD"] = req.methode;
	CGI_BOOK["QUERY_STRING"] = req.Query_String;
	// CGI_BOOK["CONTENT_LENGTH"] = req.header_request["Content-Length"];
	// CGI_BOOK["CONTENT_TYPE"] = req.header_request["Content-Type"];
	CGI_BOOK["SERVER_PROTOCOL"] = "HTTP/1.1";
	CGI_BOOK["REDIRECT_STATUS"] = "200";
	CGI_BOOK["FCGI_ROLE"] = "RESPONDER";
	CGI_BOOK["REQUEST_SCHEME"] = "http";

}

cgi_handler::~cgi_handler()
{
}