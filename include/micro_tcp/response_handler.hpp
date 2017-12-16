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

#ifndef MICRO_TCP_RESPONSE_HANDLER_HPP
#define MICRO_TCP_RESPONSE_HANDLER_HPP

#include <micro_tcp/message.hpp>
#include <micro_tcp/files.hpp>
#include <iostream>
#include <fstream>

namespace micro_tcp
{
    /**
     * @brief This class should be derived from and its functionality should be overridden where at least
     * response_handler::handle_response(const micro_tcp::message&) should be implemented.
     */
    class response_handler
    {
    public:
        response_handler() = default;

        ~response_handler() = default;

        inline virtual void handle_response(const micro_tcp::message& response)
        {
            if (response.content_buffer_.size() > 10000)
            {
                if (micro_tcp::write_file("test.out", response))
                {
                    std::cout << "CLIENT | Received content was larger then 10000. Written to file: test.out"
                              << std::endl;
                }
            }
            else
            {
                std::cout << "CLIENT | Received response: "
                          << std::string(response.content_buffer_.begin(), response.content_buffer_.end()) << std::endl;
            }
        }
    };
}

#endif
