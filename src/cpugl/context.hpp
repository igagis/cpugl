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

	// Rasterization tutorial:
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage.html

	template <bool depth_test, typename vertex_program_type, typename fragment_program_type, typename... attribute_type>
	void render(
		const vertex_program_type& vertex_program,
		const fragment_program_type& fragment_program,
		utki::span<const attribute_type>... attribute
	)
	{
		auto attrs_tuple = std::make_tuple(attribute...);
		static_assert(
			std::is_invocable_v<decltype(vertex_program), const attribute_type&...>,
			"vertex_program must be invocable"
		);

		using vertex_program_res_type = decltype( //
			vertex_program( //
				std::declval<typename decltype(attribute)::value_type>()...
			)
		);

		static_assert(
			utki::is_specialization_of_v<std::tuple, vertex_program_res_type>,
			"vertex program return type must be std::tuple"
		);

		std::array<vertex_program_res_type, 3> face{};

		auto face_i = face.begin();
		for (auto attr_iters = std::make_tuple(attribute.begin()...);
			 std::get<0>(attr_iters) != std::get<0>(attrs_tuple).end();
			 std::apply(
				 [](auto&... i) {
					 (..., ++i);
				 },
				 attr_iters
			 ))
		{
			auto vertex_program_args_tuple = std::apply(
				[](const auto&... i) {
					return std::make_tuple((*i)...);
				},
				attr_iters
			);

			*face_i = std::apply(vertex_program, vertex_program_args_tuple);
			++face_i;
			if (face_i != face.end()) {
				continue;
			}
			face_i = face.begin();

			auto bb_segment =
				calc_bounding_box_segment(std::get<0>(face[0]), std::get<0>(face[1]), std::get<0>(face[2]));

			r4::rectangle<real> bounding_box = {
				bb_segment.p1,
				bb_segment.p2 - bb_segment.p1 + decltype(bb_segment.p1)(1) // +1 because segment is [p1, p2]
			};

			auto framebuffer_span = this->framebuffer->span().subspan(bounding_box.to<unsigned>());

			auto p = bounding_box.p;
			for (auto line : framebuffer_span) {
				for (auto& px : line) {
					auto barycentric = r4::vector3<real>{
						edge_function(std::get<0>(face[1]), std::get<0>(face[2]), p),
						edge_function(std::get<0>(face[2]), std::get<0>(face[0]), p),
						edge_function(std::get<0>(face[0]), std::get<0>(face[1]), p)
					};

					if (barycentric.is_positive_or_zero()) {
						// normalize barycentric coordinates
						auto area = edge_function(std::get<0>(face[0]), std::get<0>(face[1]), std::get<0>(face[2]));
						barycentric /= area;

						// interpolate attributes

						auto interpolated_attributes = //
							[&b = barycentric, &f = face]<size_t... i>(std::index_sequence<i...>) {
								return std::make_tuple(
									(std::get<i>(f[0]) * b[0] + std::get<i>(f[1]) * b[1] + std::get<i>(f[2]) * b[2])...
								);
							}(utki::offset_sequence_t<
								1,
								std::make_index_sequence< //
									std::tuple_size_v<vertex_program_res_type> - 1 //
									> //
								>{});

						static_assert(
							utki::is_specialization_of_v<std::tuple, decltype(interpolated_attributes)>,
							"interpolated_attributes type must be std::tuple"
						);

						static_assert(
							[]<typename... arg_type>(std::tuple<arg_type...>) constexpr {
								return std::is_invocable_v<decltype(fragment_program), const arg_type&...>;
							}(decltype(interpolated_attributes){}),
							"vertex_program must be invocable"
						);

						auto pixel_color = std::apply(fragment_program, interpolated_attributes);

						px = rasterimage::to<std::remove_reference_t<decltype(px)>::value_type>(pixel_color);
					}

					++p.x();
				}
				p.x() = bounding_box.p.x();
				++p.y();
			}
		}
	}
};

} // namespace cpugl
