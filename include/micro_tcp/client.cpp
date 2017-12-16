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

#include <micro_tcp/client.hpp>
#include <micro_tcp/files.hpp>
#include <boost/asio/connect.hpp>

namespace micro_tcp
{
    client::client(boost::asio::io_service& io_service, micro_tcp::response_handler& response_handler,
                   boost::asio::ssl::context& context) :
            context_(context),
            io_strand_(io_service),
            socket_(io_service),
            resolver_(io_service),
            active_session_(nullptr),
            response_handler_(response_handler)
    {
        /*...*/
    }

    client::~client() = default;

    void client::connect(const std::string& remote_host, unsigned short remote_port)
    {
        resolver_.async_resolve({remote_host, std::to_string(remote_port)}, [this](const boost::system::error_code& ec,
                                                                                   boost::asio::ip::tcp::resolver::iterator result)
        {
            if (!ec)
            {
                boost::asio::async_connect(socket_, result, [this](boost::system::error_code ec,
                                                                   boost::asio::ip::tcp::resolver::iterator /*endpoint_connected*/)
                {
                    if (!ec)
                    {
                        active_session_ = std::make_shared<client_session>(std::move(socket_), context_, response_handler_);
                        active_session_->start();
                    }
                    else if (ec != boost::asio::error::connection_aborted)
                    {
                        std::cerr << __PRETTY_FUNCTION__ << " | " << "Client connection failed"
                                  << " | Boost asio/system error message: " << ec.message() << '\n';
                    }
                });
            }
        });
    }

    void client::disconnect()
    {
        if (is_connected())
        {
            active_session_->stop();
        }
        active_session_.reset();
    }

    bool client::send(const micro_tcp::message& message)
    {
        if (is_connected())
        {
            active_session_->send(message);
            return true;
        }
        return false;
    }

    bool client::send_file(const std::string& file_path)
    {
        micro_tcp::message file;
        if (micro_tcp::read_file(file_path, file))
        {
            return send(file);
        }
        return false;
    }

    bool client::is_connected() const
    {
        return active_session_ && active_session_->is_alive();
    }
}