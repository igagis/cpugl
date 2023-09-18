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

#include "context.hpp"

#include <r4/segment2.hpp>

using namespace cpugl;

void context::render(utki::span<const r4::vector4<real>> pos)
{
	std::array<r4::vector4<real>, 3> face{};
	auto face_i = face.begin();
	for (const auto& p : pos) {
		*face_i = p;
		++face_i;
		if (face_i != face.end()) {
			continue;
		}
		face_i = face.begin();

		using std::min;
		using std::max;

		// r4::segment2<real> bb{
		// 	{min(face[0].x(), min(face[1].x(), face[2].x())), min(face[0].y(), min(face[1].y(), face[2].y()))},
		// 	{max(face[0].x(), max(face[1].x(), face[2].x())), max(face[0].y(), max(face[1].y(), face[2].y()))},
		// };

		(*this->framebuffer)[p.y()][p.x()] = {0xff, 0, 0, 0xff};
	}
}
