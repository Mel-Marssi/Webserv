#include "cgi_handler.hpp"


int cgi_handler::i = 0;

void php_cgi(Request &req, server_config &config)
{
	//  cout << "CGI------------>" << endl;
	(void)config;
	cgi_handler cgi(req);
	string file = req.full_Path;
	time_t t = time(NULL);
	stringstream to_s;
	to_s << t;

	string tmp_file = "/tmp/_" + to_s.str();
	req.cgi_file = tmp_file;
	int fd[2];

	char **env = new char *[cgi.CGI_BOOK.size() + 1];
	map<string, string>::iterator it = cgi.CGI_BOOK.begin();
	for (int i = 0; it != cgi.CGI_BOOK.end(); it++, i++)
	{
		string tmp = it->first + "=" + it->second;
		env[i] = new char[tmp.size() + 1]();
		strcpy(env[i], tmp.c_str());
	}
	env[cgi.CGI_BOOK.size()] = NULL;

	string tmp = config.get_loc_cgi_path("/cgi-bin");
	char *argv[3] = {(char *)tmp.c_str(), (char *)file.c_str(), NULL};
	fd[0] = open(file.c_str(), O_RDONLY);
	fd[1] = open(tmp_file.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
	req.start = clock();
	req.pid = fork();
	if (req.pid == 0)
	{
		dup2(fd[0], 0);
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		execve(argv[0], argv, env);
		cerr << "error execve" << endl;
		for(int i = 0; env[i]; i++)
			delete[] env[i];
		delete[] env;
		exit(1);
	}
	close(fd[0]);
	close(fd[1]);
	for(int i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
}
cgi_handler::cgi_handler(Request &req)
{

	CGI_BOOK["SCRIPT_FILENAME"] = "." + req.Path + "/" + req.file_get;
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