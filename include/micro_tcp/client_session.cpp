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

#include <micro_tcp/client_session.hpp>

namespace micro_tcp
{
    client_session::client_session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context& context,
                                   micro_tcp::response_handler& response_handler) :
            session(std::move(socket), context),
            response_handler_(response_handler),
            timeout_(io_strand_.get_io_service())
    {
        /*...*/
    }

    void client_session::start()
    {
        do_secure_handshake(boost::asio::ssl::stream_base::client);
    }

    void client_session::send(const micro_tcp::message& message)
    {
        write_buffer_ = message;
        write_buffer_.prepare_header_buffer_write();
        do_write_header();
    }

    void client_session::on_secure_handshake()
    {
        debug("CLIENT | secure handshake OK");
        //do_timeout();
    }

    void client_session::on_write_header()
    {
        debug("CLIENT | write request header OK");
        write_buffer_.prepare_content_buffer_write();
        do_write_content();
    }

    void client_session::on_write_content()
    {
        debug("CLIENT | write request content OK");
        read_buffer_.prepare_header_buffer_read();
        do_read_header();
    }

    void client_session::on_read_header()
    {
        debug("CLIENT | read response header OK");
        read_buffer_.prepare_content_buffer_read();
        do_read_content();
    }

    void client_session::on_read_content()
    {
        debug("CLIENT | read response content OK");
        response_handler_.handle_response(read_buffer_);
        read_buffer_.clear();
        write_buffer_.clear();
        //set_timeout_expiry_time();
    }

    void client_session::on_shutdown_secure_stream()
    {
        debug("CLIENT | shutting down secure (SSL/TLS) protocol on stream OK");
        do_close_socket();
    }

    void client_session::on_close_socket()
    {
        debug("CLIENT | socket close OK");
    }

    void client_session::do_timeout()
    {
        timeout_.async_wait(io_strand_.wrap([this](const boost::system::error_code& ec)
        {
            if (!ec)
            {
                stop();
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                debug("async_wait failed", ec.message());
            }
        }));
    }

    void client_session::set_timeout_expiry_time(unsigned long timeout_ms)
    {
        boost::system::error_code ec;
        if (timeout_.expires_from_now(boost::posix_time::seconds(timeout_ms), ec) > 0)
        {
            do_timeout();
        }
        else
        {
            debug("timer has already expired or was never started at all", " ");
        }
    }
}