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

#include "config.hpp"

namespace cpugl {

class context
{
	static r4::segment2<real> calc_bounding_box_segment(
		const r4::vector2<real>& v0,
		const r4::vector2<real>& v1,
		const r4::vector2<real>& v2
	)
	{
		using std::min;
		using std::max;

		return {min(v0, min(v1, v2)), max(v0, max(v1, v2))};
	}

	static real edge_function(const r4::vector2<real>& a, const r4::vector2<real>& b, const r4::vector2<real>& c)
	{
		return (c - a).cross(b - a);
	}

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

	// template <typename first_arg_type, typename ... rest_args_type>
	// std::tuple<rest_args_type...> subtuple(first_arg_type, rest_args_type... args){
	// 	return std::make_tuple(args...);
	// }

	template <bool depth_test, typename vertex_program_type, typename fragment_program_type, typename ... attribute_type>
	void render(
		const vertex_program_type& vertex_program,
		const fragment_program_type& fragment_program,
		utki::span<const attribute_type>... attribute
	)
	{
		auto attrs_tuple = std::make_tuple(attribute...);
		static_assert(std::is_invocable_v<decltype(vertex_program), const attribute_type&...>, "vertex_program must be invocable");

		// TODO:
		// static_assert(std::is_invocable_v<decltype(fragment_program)>, "fragment_program must be invocable");

		const auto& pos = std::get<0>(attrs_tuple);

		std::array<r4::vector4<real>, 3> face{};
		auto face_i = face.begin();
		for (const auto& vertex : pos) {
			*face_i = vertex;
			++face_i;
			if (face_i != face.end()) {
				continue;
			}
			face_i = face.begin();

			auto bb_segment = calc_bounding_box_segment(face[0], face[1], face[2]);

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
};

} // namespace cpugl
