//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "spdlog/spdlog.h"

using boost::asio::ip::tcp;

class session
	: public std::enable_shared_from_this<session>
{
public:
	session(boost::asio::io_context& io_context, boost::asio::ip::tcp::endpoint&& endpoints)
		: io_context_(io_context),
		endpoints_(std::move(endpoints))
	{
	}

	void start()
	{
		socket_ = new tcp::socket(io_context_);
		socket_->set_option(boost::asio::ip::tcp::no_delay(true));
		socket_->async_connect(endpoints_,
			boost::bind(&session::do_connect, shared_from_this(), boost::asio::placeholders::error));
	}

private:
	void do_connect(const boost::system::error_code& error)
	{
		if (error)
		{
			spdlog::warn("{} error: {}", __FUNCTION__, error.message().c_str());
			return;
		}
		memset(data_, 0, 6);;
		memcpy(data_, "12345", 6);
		do_write(5);
	}

	void do_read()
	{
		auto self(shared_from_this());
		socket_->async_read_some(boost::asio::buffer(data_, max_length),
			[this, self](boost::system::error_code ec, std::size_t length)
			{
				if (ec)
				{
					spdlog::warn("{} error: {}", __FUNCTION__, ec.message().c_str());
					return;
				}
				self->do_read();
			});
	}

	void do_write(std::size_t length)
	{
		auto self(shared_from_this());
		boost::asio::async_write(*socket_, boost::asio::buffer(data_, length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (ec)
				{
					spdlog::warn("{} error: {}", __FUNCTION__, ec.message().c_str());
					return;
				}
				do_read();
			});
	}
	boost::asio::io_context& io_context_;
	tcp::socket* socket_;
	boost::asio::ip::tcp::endpoint endpoints_;
	enum { max_length = 1024 };
	char data_[max_length] = {0};
};

class manager
{
public:
	manager(boost::asio::io_context& io_context,const std::string& ip, short port, short count)
		: io_context_(io_context), 
		endpoints_(boost::asio::ip::address::from_string(ip), port),
		count_(count)
	{
		do_connect();
	}

private:
	void do_connect()
	{
		for (auto i = 0; i < count_; i++)
		{
			std::make_shared<session>(io_context_,std::move(endpoints_))->start();
		}
	}

	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::endpoint endpoints_;
	short count_;

};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 4)
		{
			std::cerr << "Usage: async_tcp_echo_client <ip> <port> <count>\n";
			return 1;
		}

		boost::asio::io_context io_context;

		manager m(io_context, argv[1], std::atoi(argv[2]), std::atoi(argv[3]));
		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
