///
//! @copyright Copyright (c) 2017 Stefan Broekman.
//! @license This file is released under the MIT license.
//! @see https://stefanbroekman.nl
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
///

#include <micro_tcp/server.hpp>
#include <micro_tcp/server_session.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <algorithm>
#include <boost/date_time.hpp>

namespace micro_tcp
{
    server::server(boost::asio::io_service& io_service, const std::string& address, unsigned short port,
                   micro_tcp::request_handler& request_handler, boost::asio::ssl::context& context,
                   const std::string& cipher_suite) :
            context_(context),
            io_strand_(io_service),
            acceptor_(io_service),
            socket_(io_service),
            endpoint_(boost::asio::ip::address::from_string(address), port),
            request_handler_(request_handler)
    {
        SSL_CTX_set_cipher_list(context_.native_handle(), cipher_suite.c_str());
    }

    server::server(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint,
                   micro_tcp::request_handler& request_handler, boost::asio::ssl::context& context,
                   const std::string& cipher_suite) :
            context_(context),
            io_strand_(io_service),
            acceptor_(io_service),
            socket_(io_service),
            endpoint_(endpoint),
            request_handler_(request_handler)
    {
        SSL_CTX_set_cipher_list(context_.native_handle(), cipher_suite.c_str());
    }

    server::~server()
    {
        stop();
    }

    void server::start()
    {
        start_listening();
    }

    void server::stop()
    {
        acceptor_.close();
    }

    bool server::is_listening()
    {
        return acceptor_.is_open();
    }

    void server::do_accept()
    {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec)
        {
            if (!acceptor_.is_open())
            {
                debug("SERVER | stopped listening");
                return;
            }
            if (!ec)
            {
                std::make_shared<server_session>(std::move(socket_), context_, request_handler_)->start();
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                debug("Error on asynchronous accept", ec.message());
            }
            do_accept();
        });
    }

    void server::start_listening()
    {
        if (!acceptor_.is_open())
        {
            boost::system::error_code ec;
            acceptor_.open(endpoint_.protocol(), ec);
            if (ec)
            {
                debug("Opening acceptor failed", ec.message());
            }
            acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
            if (ec)
            {
                debug("Setting acceptor option failed", ec.message());
            }
            acceptor_.bind(endpoint_, ec);
            if (ec)
            {
                debug("Binding acceptor to local endpoint failed", ec.message());
            }
            acceptor_.listen(boost::asio::socket_base::max_connections, ec);
            if (ec)
            {
                debug("Start listening for new connections failed", ec.message());
            }
            std::ostringstream oss;
            oss << "SERVER | started listening on <" << get_address_port() << ">.";
            debug(oss.str());
            do_accept();
        }
    }

    bool server::set_address(const std::string& address)
    {
        boost::system::error_code ec;
        auto address_checked = boost::asio::ip::address::from_string(address, ec);
        return !ec && set_address(address_checked);
    }

    bool server::set_address(const boost::asio::ip::address& address)
    {
        endpoint_.address(address);
        if (acceptor_.is_open())
        {
            debug("Could not change the address", "Stop the server before making any changes");
            return false;
        }
        endpoint_.address(address);
        debug("SERVER | address set to " + get_address());
        return true;
    }

    std::string server::get_address() const
    {
        return endpoint_.address().to_string();
    }

    bool server::set_port(unsigned short port)
    {
        if (acceptor_.is_open())
        {
            debug("Could not change the port", "Stop the server before making any changes");
            return false;
        }
        if (port_in_use(port))
        {
            debug("Could not change the port", "port " + std::to_string(port) + " is already in use by another service");
            return false;
        }
        endpoint_.port(port);
        debug("SERVER | port set to " + std::to_string(get_port()));
        return true;
    }

    unsigned short server::get_port() const
    {
        return endpoint_.port();
    }

    std::string server::get_address_port() const
    {
        std::ostringstream oss;
        oss << get_address() << ':' << get_port();
        return oss.str();
    }

    bool server::set_endpoint(const boost::asio::ip::tcp::endpoint& endpoint)
    {
        if (acceptor_.is_open())
        {
            debug("Could not change the endpoint", "Stop the server before making any changes");
            return false;
        }
        endpoint_ = endpoint;
        debug("SERVER | endpoint set to <" + get_address_port() + ">.");
        return true;
    }

    bool server::set_request_handler(micro_tcp::request_handler& request_handler)
    {
        if (acceptor_.is_open())
        {
            debug("Could not change the request_handler", "Stop the server before making any changes");
            return false;
        }
        request_handler_ = request_handler;
        return true;
    }

    bool server::port_in_use(unsigned short port)
    {
        boost::asio::ip::tcp::acceptor acceptor(io_strand_.get_io_service());
        boost::asio::ip::tcp::endpoint endpoint(endpoint_.address(), port);
        boost::system::error_code ec;
        acceptor.open(endpoint.protocol(), ec) || acceptor.bind(endpoint, ec);
        acceptor.close();
        return (ec == boost::asio::error::address_in_use);
    }

    std::vector<std::string> server::get_local_addresses(bool only_ipv4)
    {
        std::vector<std::string> addresses;
        boost::asio::ip::tcp::resolver resolver(io_strand_.get_io_service());
        std::string hostname = boost::asio::ip::host_name();
        std::for_each(resolver.resolve({hostname, ""}), {}, [this, only_ipv4, &addresses](const auto& re)
        {
            const auto address = re.endpoint().address();
            if (address.is_v4())
            {
                addresses.push_back(address.to_string());
            }
            else if ((address.is_v6() && !only_ipv4))
            {
                addresses.push_back(address.to_string());
            }
        });
        return addresses;
    }

    void server::debug(const std::string& msg, const std::string& ec)
    {
        std::ostringstream oss;
        boost::posix_time::time_facet *facet(new boost::posix_time::time_facet(std::string("%Y-%m-%d %H:%M:%S.%f").c_str()));
        oss.imbue(std::locale(oss.getloc(), facet));
        oss << boost::posix_time::microsec_clock::local_time() << " | " << '[' << "host:port" << "] | " << msg;
        if (!ec.empty())
        {
            oss << " | Error code: " << ec;
            std::cerr << oss.str() << '\n';
        }
        else
        {
            std::cout << oss.str() << '\n';
        }
    }
}