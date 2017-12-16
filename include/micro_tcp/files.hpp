///
//! @copyright Copyright 2017 Stefan Broekman. All rights reserved.
//! @license This file is released under the MIT license.
//! @see https://broekman.biz
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

#ifndef MICRO_TCP_FILES_HPP
#define MICRO_TCP_FILES_HPP

#include <micro_tcp/message.hpp>
#include <fstream>
#include <iostream>

namespace micro_tcp
{
    inline bool read_file(const std::string& file_path, micro_tcp::message& message)
    {
        std::ifstream file(file_path, std::ios::in | std::ios::binary);
        if (file)
        {
            file.seekg(0, std::ios::end);
            message.content_buffer_.resize(file.tellg());
            file.seekg(0, std::ios::beg);
            file.read(message.content_buffer_.data(), message.content_buffer_.size());
            file.close();
            return true;
        }
        std::cerr << __PRETTY_FUNCTION__ << " | " << "The file (" << file_path << ") could not be read!\n";
        return false;
    }

    inline bool write_file(const std::string& file_path, const micro_tcp::message& message)
    {
        std::ofstream output_file(file_path, std::ios::out | std::ios::binary);
        if (output_file)
        {
            output_file.write(message.content_buffer_.data(), message.content_buffer_.size());
            output_file.close();
            return true;
        }
        std::cerr << __PRETTY_FUNCTION__ << " | " << "The file (" << file_path << ") could not be written!\n";
        return false;
    }
}

#endif
