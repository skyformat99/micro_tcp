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

#include <micro_tcp/message.hpp>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>

namespace micro_tcp
{
    /*static*/constexpr std::array<message::buffer_type::value_type, 18> message::magic_numbers_;

    message::message()
    {
        prepare_header_buffer_read();
    }

    message::message(const std::string& content)
    {
        set_content_buffer(content);
    }

    message::message(const buffer_type& content)
    {
        set_content_buffer(content);
    }

    message::~message() = default;

    void message::prepare_header_buffer_write()
    {
        header_buffer_.assign(magic_numbers_.begin(), magic_numbers_.end());
        std::stringstream content_length_buffer;
        content_length_buffer << std::noskipws << std::setw(content_length_digits10_) << content_buffer_.size();
        std::copy(std::istream_iterator<buffer_type::value_type>(content_length_buffer),
                  std::istream_iterator<buffer_type::value_type>(),
                  std::back_inserter(header_buffer_));
    }

    void message::prepare_content_buffer_write()
    {
        if (content_buffer_.size() != get_header_buffer_content_length())
        {
            prepare_header_buffer_write();
        }
    }

    void message::prepare_header_buffer_read()
    {
        clear_header_buffer();
        header_buffer_.resize(magic_numbers_.size() + content_length_digits10_);
    }

    void message::prepare_content_buffer_read()
    {
        clear_content_buffer();
        content_buffer_.resize(get_header_buffer_content_length());
    }

    std::size_t message::get_header_buffer_content_length()
    {
        if (header_buffer_.size() != message::default_header_length())
        {
            return 0;
        }

        std::stringstream content_length_buffer;
        content_length_buffer << std::setw(content_length_digits10_);
        std::copy(header_buffer_.begin() + magic_numbers_.size(),
                  header_buffer_.end(),
                  std::ostream_iterator<buffer_type::value_type>(content_length_buffer));
        if (content_length_buffer.good())
        {
            std::size_t content_length;
            content_length_buffer >> content_length;
            return content_length;
        }
        return 0;
    }

    void message::clear()
    {
        clear_header_buffer();
        clear_content_buffer();
    }

    void message::clear_header_buffer()
    {
        header_buffer_.clear();
        header_buffer_.shrink_to_fit();
    }

    void message::clear_content_buffer()
    {
        content_buffer_.clear();
        content_buffer_.shrink_to_fit();
    }

    void message::set_content_buffer(const std::string& content)
    {
        content_buffer_.assign(content.begin(), content.end());
        prepare_header_buffer_write();
    }

    void message::set_content_buffer(const buffer_type& content)
    {
        content_buffer_ = content;
        prepare_header_buffer_write();
    }

    /*static*/std::size_t message::default_header_length()
    {
        return magic_numbers_.size() + content_length_digits10_;
    }
}