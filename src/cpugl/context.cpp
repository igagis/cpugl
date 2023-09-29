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

r4::segment2<real> calc_bounding_box_segment(const std::array<r4::vector4<real>, 3>& face)
{
	using std::min;
	using std::max;

	return {
		{min(face[0].x(), min(face[1].x(), face[2].x())), min(face[0].y(), min(face[1].y(), face[2].y()))},
		{max(face[0].x(), max(face[1].x(), face[2].x())), max(face[0].y(), max(face[1].y(), face[2].y()))},
	};
}

real edge_function(const r4::vector2<real>& a, const r4::vector2<real>& b, const r4::vector2<real>& c)
{
	return (c - a).cross(b - a);
}

void context::render(utki::span<const r4::vector4<real>> pos)
{
	std::array<r4::vector4<real>, 3> face{};
	auto face_i = face.begin();
	for (const auto& vertex : pos) {
		*face_i = vertex;
		++face_i;
		if (face_i != face.end()) {
			continue;
		}
		face_i = face.begin();

		auto bb_segment = calc_bounding_box_segment(face);

		r4::rectangle<real> bb = {
			bb_segment.p1,
			bb_segment.p2 - bb_segment.p1 + decltype(bb_segment.p1)(1) // +1 because segment is [p1, p2]
		};

		auto framebuffer_span = this->framebuffer->span().subspan(bb.to<unsigned>());

		auto p = bb.p;
		for (auto line : framebuffer_span) {
			for (auto& px : line) {
				auto w0 = edge_function(face[1], face[2], p);
				auto w1 = edge_function(face[2], face[0], p);
				auto w2 = edge_function(face[0], face[1], p);

				if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
					px = {0, 0xff, 0, 0xff}; // NOLINT
				}

				++p.x();
			}
			p.x() = bb.p.x();
			++p.y();
		}
	}
}
