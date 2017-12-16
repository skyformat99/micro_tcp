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

#ifndef MICRO_TCP_MESSAGE_HPP
#define MICRO_TCP_MESSAGE_HPP

#include <vector>
#include <array>

namespace micro_tcp
{
    /**
     * @brief
     */
    struct message
    {
        typedef std::vector<char> buffer_type;

        /**
         * @brief
         */
        static constexpr std::array<buffer_type::value_type, 18> magic_numbers_ = {'/', 'b', 'r', 'o', 'e', 'k', 'm', 'a', 'n', '/', 't', 'c', 'p', '/', '1', '.', '0', '/'};
        /**
         * @brief
         */
        static constexpr auto content_length_digits10_ = 10;

        /**
         * @brief
         */
        explicit message();

        /**
         * @brief
         * @param content
         */
        explicit message(const std::string& content);

        /**
         * @brief
         * @param content
         */
        explicit message(const buffer_type& content);

        /**
         * @brief
         */
        ~message();

        /**
         * @brief
         */
        void prepare_header_buffer_write();

        /**
         * @brief
         */
        void prepare_content_buffer_write();

        /**
         * @brief
         */
        void prepare_header_buffer_read();

        /**
         * @brief
         */
        void prepare_content_buffer_read();

        /**
         * @brief
         */
        void clear();

        /**
         * @brief
         */
        void clear_header_buffer();

        /**
         * @brief
         */
        void clear_content_buffer();

        /**
         * @brief
         *
         * @return
         */
        std::size_t get_header_buffer_content_length();

        /**
         * @brief
         *
         * @param content
         */
        void set_content_buffer(const std::string& content);

        /**
         * @brief
         *
         * @param content
         */
        void set_content_buffer(const buffer_type& content);

        /**
         *
         * @return
         */
        static std::size_t default_header_length();

        buffer_type header_buffer_; /*!< Buffer used for incoming/outgoing header bytes. */
        buffer_type content_buffer_; /*!< Buffer used for incoming/outgoing content bytes. */
    };
}

#endif