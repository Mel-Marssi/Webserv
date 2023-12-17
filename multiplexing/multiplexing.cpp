#include "multiplexing.hpp"

multiplexing::multiplexing( servers &config)
{
	int socket_fd[(int)config.size()];
	struct sockaddr_in bind_socket;
	int socket_client;
	socklen_t  lenght;
		lenght = sizeof(bind_socket);
	for(int i = 0; i < (int)config.size(); i++)
	{
		socket_fd[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd[i] < 0)
			throw (runtime_error("socket call failed!"));
		bind_socket.sin_addr.s_addr = INADDR_ANY;
		bind_socket.sin_family = AF_INET;
		bind_socket.sin_port = htons(config[i].get_port());
		try
		{
			if(bind(socket_fd[i], (struct sockaddr*)&bind_socket, sizeof(bind_socket)))
				throw (runtime_error("bind call failed!"));
			if (listen(socket_fd[i], 3) < 0)
				throw (runtime_error("listen call failed!"));
			if((socket_client = accept(socket_fd[i], NULL, NULL)) < 0)
				throw (runtime_error("accept call failed!"));
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
		
		// int fd = epoll_create();
		// epoll_ctl(fd, EPOLL_CTL_ADD, socket_client, )
		// fd_socket.push_back(socket_client);
	}
}

multiplexing::~multiplexing()
{

}
