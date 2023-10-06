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

#include "color_pos_shader.hpp"

using namespace cpugl;

void color_pos_shader::render(
	context& ctx,
	const r4::matrix4<real>& matrix,
	cpugl::context::fb_image_type::pixel_type color,
	utki::span<const r4::vector4<real>> pos
)
{
	ctx.render<false>( // false = no depth test
		[&matrix](const std::tuple<const r4::vector4<real>&>& attribute) {
			const auto& pos = std::get<0>(attribute);
			return std::make_tuple(matrix * pos);
		},
		[]() {
			return r4::vector4<uint8_t>{0xff, 0, 0, 0xff};
		},
		pos
	);
}
