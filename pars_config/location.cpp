#include "location.hpp"

location::location()
{
	_get = _post = _auto_index = _delete = _allow_upload = false;
	// path_location = "";
	index = "";
	root = "";
	redirection = "";
	uploads_folder = "";
	max_client_size = 0;
}
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
bool location::get_upload() const
{
	return (_allow_upload);
}
size_t location::get_max_client_size() const
{
	return (max_client_size);
}
string location::get_root() const
{
	return (root);
}
string location::get_upload_folder() const
{
	return (uploads_folder);
}
string location::get_index() const
{
	return (index);
}
string location::get_redirection() const
{
	return (redirection);
}
// string location::get_cgi_path() const
// {
// 	return (cgi_path);
// }
map<string, string> location::get_cgi_exec_path() const
{
	return (cgi_script);
}
// string location::get_path_location() const
// {
// 	return (path_location);
// }

ostream &operator<<(ostream &var, const location &obj)
{
	// var << "path_location: " << obj.get_path_location() << "\n";
	var << "get: " << obj.get() << "\n";
	var << "post: " << obj.get_post() << "\n";
	var << "root: " << obj.get_root() << "\n";
	var << "delete: " << obj.get_delete() << "\n";
	var << "index: " << obj.get_index() << "\n";
	var << "autoindex: " << obj.get_auto_index() << "\n";
	var << "redirection: " << obj.get_redirection() << "\n";
	// var << "cgi_path: " << obj.get_cgi_path() << endl;
	return (var);
}
location::~location()
{
}

location::location(ifstream &config_fd, string word_serv)
{
	if (word_serv == "location" || word_serv == "{")
		throw(runtime_error("Invalide configue file!"));
	_get = _post = _auto_index = _delete = _allow_upload = false;
	index = "";
	root = "";
	redirection = "";
	uploads_folder = "";
	max_client_size = 0;
	this->setup_location(config_fd);
}

void location::setup_location(ifstream &config_fd)
{
	string file, word;
	while (getline(config_fd, file))
	{
		if (file.find(";") != string::npos)
		{
			file.insert(file.find(";"), " ");
			if (file.find(";") != file.size() - 1)
				throw(runtime_error("Character after \";\""));
		}
		istringstream cscan(file);
		cscan >> word;
		if (word == "}")
			break;
		if (word == "allow_methods")
		{
			if (_get || _post || _delete)
			{
				throw(runtime_error("Invalide configue file!"));
			}
			while (word != ";")
			{
				cscan >> word;
				if (word == "GET")
					_get = true;
				else if (word == "POST")
					_post = true;
				else if (word == "DELETE")
					_delete = true;
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
		else if (word == "root")
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
				throw(runtime_error("Invalid configue file!"));
			cscan >> word;
			index = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid location!");
		}
		else if (word == "return")
		{
			if (!redirection.empty())
				throw(runtime_error("Invalid configue file!"));
			cscan >> word;
			redirection = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid location!");
		}
		else if (word == "max_client_size")
		{
			if (max_client_size != 0)
				throw(runtime_error("Invalid configue file!"));
			cscan >> word;
			check_size_t(word.c_str(), "Invalid location!");
			stringstream convert(word);
			ostringstream checks;
			convert >> max_client_size;
			checks << max_client_size;
			if (word != checks.str())
				throw(runtime_error("Invalid location!"));
			cscan >> word;
			if (max_client_size == 0 || word != ";")
				throw(runtime_error("Invalid configue file!"));
		}
		else if (word == "allow_upload")
		{
			if (_allow_upload)
				throw(runtime_error("Invalide configue file!"));
			cscan >> word;
			if (word == "on")
				_allow_upload = true;
			else if (word != "off")
				throw(runtime_error("Invalid allow_upload!"));
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid location!");
		}
		else if (word == "uploads_folder")
		{
			if (!uploads_folder.empty())
				throw(runtime_error("Invalide configue file!"));
			cscan >> word;
			uploads_folder = word;
			if (access(uploads_folder.c_str(), F_OK) == -1)
				throw(runtime_error("Invalid uploads_folder!"));
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid location!");
		}
		else if (word == "cgi_path")
		{
			if (file.find(";") == string::npos)
				throw(runtime_error("Invalid configue file!"));
			cscan >> word;
			while (word != ";")
			{
				extention = word;
				if (extention[0] != '.')
					throw(runtime_error("Invalid extention on cgi_path!"));
				cscan >> word;
				cgi_script[extention] = word;
				if (access(cgi_script[extention].c_str(), F_OK) == -1)
					throw(runtime_error("Invalid cgi_path!"));
				cscan >> word;
			}
		}
		else if (word.empty())
			throw runtime_error("Invalid location!");
		else if (word == ";")
			continue;
		else
			throw runtime_error("Invalid location!1");
	}
	if (word != "}" || (uploads_folder.empty() && _allow_upload == true))
		throw runtime_error("Invalid location!");
}

void check_size_t(const char *av, const char *message)
{
	for (int i = 0; i < (int)strlen(av); i++)
	{
		if (av[i] < '0' || av[i] > '9')
			throw(runtime_error(message));
	}
}

int check_atoi(const char *av, const char *message)
{
	long long tmp;
	for (int i = 0; i < (int)strlen(av) - 1; i++)
	{
		if (av[i] < '0' || av[i] > '9')
		{
			throw(runtime_error(message));
		}
	}
	tmp = atol(av);
	if (tmp > 65535 || tmp < 0)
		throw(runtime_error(message));
	return (static_cast<int>(tmp));
}
