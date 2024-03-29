/*
MIT License

Copyright (c) 2023 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <r4/segment2.hpp>
#include <rasterimage/image.hpp>
#include <utki/types.hpp>

#include "config.hpp"

namespace cpugl {

class context
{
public:
	using fb_image_type = rasterimage::image<uint8_t, 4>;

private:
	fb_image_type* framebuffer = nullptr;

public:
	void set_framebuffer(fb_image_type& fb)
	{
		this->framebuffer = &fb;
	}

	void clear(fb_image_type::pixel_type color)
	{
		if (!this->framebuffer) {
			return;
		}
		this->framebuffer->span().clear(color);
	}

	fb_image_type& get_framebuffer()
	{
		ASSERT(this->framebuffer)
		return *this->framebuffer;
	}
};

} // namespace cpugl
