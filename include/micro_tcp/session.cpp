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

#include <micro_tcp/session.hpp>
#include <boost/asio/read.hpp>
#include <iostream>
#include <boost/date_time.hpp>

namespace micro_tcp
{
    session::session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context& context) :
            socket_(std::move(socket)),
            secure_stream_(socket_, context),
            io_strand_(secure_stream_.get_io_service())
    {
        /*...*/
    }

    session::~session() = default;

    void session::do_secure_handshake(boost::asio::ssl::stream_base::handshake_type type)
    {
        auto self(shared_from_this());
        secure_stream_.async_handshake(type, io_strand_.wrap([this, self](boost::system::error_code ec)
        {
            if (!ec)
            {
                on_secure_handshake();
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                debug("Error on secure handshake", ec.message());
                do_close_socket();
            }
        }));
    }

    void session::do_read_header()
    {
        auto self(shared_from_this());
        boost::asio::async_read(secure_stream_, boost::asio::buffer(read_buffer_.header_buffer_), io_strand_.wrap([this, self](
                boost::system::error_code ec, std::size_t /*bytes_transferred*/)
        {
            if (!ec)
            {
                on_read_header();
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                if (ec != boost::asio::error::eof)
                {
                    debug("Error reading content", ec.message());
                }
                stop();
            }
        }));
    }

    void session::do_read_content()
    {
        auto self(shared_from_this());
        boost::asio::async_read(secure_stream_, boost::asio::buffer(read_buffer_.content_buffer_), io_strand_.wrap([this, self](
                boost::system::error_code ec, std::size_t /*bytes_transferred*/)
        {
            if (!ec)
            {
                on_read_content();
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                if (ec != boost::asio::error::eof)
                {
                    debug("Error reading content", ec.message());
                }
                stop();
            }
        }));
    }

    void session::do_write_header()
    {
        auto self(shared_from_this());
        boost::asio::async_write(secure_stream_, boost::asio::buffer(write_buffer_.header_buffer_, write_buffer_.header_buffer_.size()), io_strand_.wrap([this, self](
                boost::system::error_code ec, std::size_t /*bytes_transferred*/)
        {
        if (!ec)
        {
            on_write_header();
        }
        else if (ec != boost::asio::error::operation_aborted)
        {
            debug("Error writing header", ec.message());
            stop();
        }
        }));
    }

    void session::do_write_content()
    {
        auto self(shared_from_this());
        boost::asio::async_write(secure_stream_, boost::asio::buffer(write_buffer_.content_buffer_, write_buffer_.content_buffer_.size()), io_strand_.wrap([this, self](
                boost::system::error_code ec, std::size_t /*bytes_transferred*/)
        {
            if (!ec)
            {
                on_write_content();
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                debug("Error writing content", ec.message());
                stop();
            }
        }));
    }

    void session::stop()
    {
        boost::system::error_code ec;
        socket().cancel(ec);
        if (ec)
        {
            debug("Error cancelling socket", ec.message());
        }
        do_shutdown_secure_stream();
    }

    void session::do_shutdown_secure_stream()
    {
        auto self(shared_from_this());
        secure_stream_.async_shutdown(io_strand_.wrap([this, self](const boost::system::error_code& ec)
        {
            if (ec.category() != boost::asio::error::get_ssl_category())
            {
                on_shutdown_secure_stream();
            }
            else
            {
                debug("Failed to securely shut down the secure (SSL/TLS) protocol on the stream", ec.message());
                do_close_socket();
            }
        }));
    }

    void session::do_close_socket()
    {
        boost::system::error_code ec;
        socket().close(ec);
        if (!ec)
        {
            on_close_socket();
        }
        else
        {
            debug("Error closing socket", ec.message());
        }
    }

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::next_layer_type& session::socket()
    {
        return secure_stream_.next_layer();
    }

    bool session::is_alive()
    {
        return socket().is_open();
    }

    void session::debug(const std::string& msg, const std::string& ec)
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