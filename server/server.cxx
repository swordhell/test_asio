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
#include "spdlog/spdlog.h"

using boost::asio::ip::tcp;

int connect_count = 0;

class session
	: public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket)
		: socket_(std::move(socket))
	{
		connect_count++;
	}

	virtual ~session()
	{
		connect_count--;
	}

	void start()
	{
		do_read();
	}

private:
	void do_read()
	{
		auto self(shared_from_this());
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			[this, self](boost::system::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					//spdlog::info("recv data: {}", data_);
					do_write(length);
				}
			});
	}

	void do_write(std::size_t length)
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					do_read();
				}
			});
	}

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length] = {0};
};

class server
{
public:
	server(boost::asio::io_context& io_context, short port)
		: acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),interval_(5000)
	{
		timer_ = std::make_shared<boost::asio::deadline_timer>(io_context, interval_);
		timer_->async_wait([this](const boost::system::error_code& e) {
			this->on_timer(e);
			});

		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket)
			{
				if (!ec)
				{
					std::make_shared<session>(std::move(socket))->start();
				}
				
				do_accept();
			});
	}

	void on_timer(const boost::system::error_code& e)
	{
		spdlog::info("client count: {}", connect_count);
		timer_->expires_at(timer_->expires_at() + interval_);
		timer_->async_wait([this](const boost::system::error_code& e) {
			this->on_timer(e);
			});
	}

	tcp::acceptor acceptor_;
	std::shared_ptr<boost::asio::deadline_timer> timer_;
	boost::posix_time::milliseconds interval_;
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: async_tcp_echo_server <port>\n";
			return 1;
		}
		boost::asio::io_context io_context;
		server s(io_context, std::atoi(argv[1]));
		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}