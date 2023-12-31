#include "location.hpp"


bool location::get_delete() const
{
	return (_delete);
}
bool location::get_post() const
{
	return (_post);
}
bool location::get() const
{
	return (_get);
}
bool location::get_auto_index() const
{
	return (_auto_index);
}
string location::get_root() const
{
	return (root);
}
string location::get_index() const
{
	return (index);
}
string location::get_redirection() const
{
	return (redirection);
}
string location::get_cgi_path() const
{
	return (cgi_path);
}
string location::get_path_location() const
{
	return (path_location);
}


ostream &operator<<(ostream &var,const location &obj)
{
	var << "path_location: " << obj.get_path_location() << "\n";
	var << "get: " << obj.get() << "\n";
	var << "post: " <<  obj.get_post() << "\n";
	var << "root: " <<  obj.get_root() << "\n";
	var << "delete: " << obj.get_delete() << "\n";
	var << "index: " << obj.get_index() << "\n";
	var << "autoindex: " << obj.get_auto_index() << "\n";
	var << "redirection: " << obj.get_redirection() << "\n";
	var << "cgi_path: " << obj.get_cgi_path()<< endl;
	return(var);
}
location::~location()
{
}


location::location(ifstream &config_fd, string &word_serv)
{
	string file, word;
	_get = _post = _auto_index = _delete = false;
	if (word_serv == "location" || word_serv == "{")
		throw(runtime_error("Invalide configue file!"));
	path_location = word_serv;
	index = "";
	root = "";
	redirection = "";
	
	while (getline(config_fd, file))
	{
		istringstream cscan(file);
		cscan >> word;
		if (word == "}")
			break;
		if (word == "allow_methods")
		{
			if (_get|| _post|| _delete)
			{	throw(runtime_error("Invalide configue file!"));}
			while (word != ";")
			{
				cscan >> word;
				if (word == "GET")
					_get = true;
				else if (word == "POST")
					_post = true;
				else if (word == "DELETE")
					_delete =  true;
				else if (word != ";")
					throw(runtime_error("Invalide configue file!"));
			}
		}
		else if (word == "autoindex")
		{
			if (_auto_index)
				throw(runtime_error("Invalide configue file!"));
			cscan >> word;
			if (word == "on")
				_auto_index = true;
			else if (word != "off")
				throw(runtime_error("Invalid autoindex!"));
			cscan >> word;
				if (word != ";")
					throw runtime_error("Invalid location!");
		}
		else if(word == "root")
		{
			if (!root.empty())
				throw(runtime_error("Invalide configue file!"));
			cscan >> word;
			root = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid location!");
		}
		else if (word == "index")
		{
			if (!index.empty())
				throw(runtime_error("Invalide configue file!"));
			cscan >> word;
			index = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid location!");
		}
		else if(word == "return")
		{
			if (!redirection.empty())
				throw(runtime_error("Invalide configue file!"));
			cscan >> word;
			redirection = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid location!");
		}
		else if (word == "cgi_path")
		{
			cscan >> word;
			cgi_path = word;
			//tanwseel double cgi
		}
		// else if (word == "uploads")
		// {

		// }
		else if (word.empty())
		 	throw runtime_error("Invalid location!");
		else if (word == ";")
			continue;
		else
			throw runtime_error("Invalid location!");
			
	}
	if (word != "}")
		throw runtime_error("Invalid location!");
}

int check_atoi(const char *av, const char *message)
{
	long long tmp;
	for(int i = 0; i < (int)strlen(av);i++)
	{
		if(av[i] < '0' || av[i] > '9')
			throw(runtime_error(message)); 
	}
	tmp =  atol(av);
	if(tmp > 65535 || tmp < 0)
		throw(runtime_error(message));
	return (static_cast<int>(tmp));
}