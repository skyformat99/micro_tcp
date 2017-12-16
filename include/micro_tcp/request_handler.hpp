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

#ifndef MICRO_TCP_REQUEST_HANDLER_HPP
#define MICRO_TCP_REQUEST_HANDLER_HPP

#include <micro_tcp/message.hpp>

namespace micro_tcp
{
    /**
     * @brief This class should be derived from and its functionality should be overridden where at least
     * request_handler::handle_request(const micro_tcp::message&, broekman::tcp_ip::message&) should be
     * implemented.
     */
    class request_handler
    {
    public:
        request_handler() = default;

        ~request_handler() = default;

        /**
         * @brief
         *
         * @param request
         * @param response
         */
        inline virtual void handle_request(const micro_tcp::message& request, micro_tcp::message& response)
        {
            response.set_content_buffer(request.content_buffer_); //Echo
        }
    };
}

#endif