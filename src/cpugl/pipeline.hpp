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

#include "config.hpp"
#include "context.hpp"
#include "mesh.hpp"

namespace cpugl {

class pipeline
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

	static real edge_function(const r4::vector2<real>& edge, const r4::vector2<real>& vec)
	{
		return vec.cross(edge);
	}

public:
	// Rasterization tutorial:
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage.html

	template <bool depth_test, typename vertex_program_type, typename fragment_program_type, typename... attribute_type>
	static void render(
		context& ctx,
		const vertex_program_type& vertex_program,
		const fragment_program_type& fragment_program,
		const mesh<attribute_type...>& mesh
	)
	{
		static_assert(
			[]<typename... arg_type>(std::tuple<arg_type...>) constexpr {
				return std::is_invocable_v<decltype(vertex_program), const r4::vector4<real>&, const arg_type&...>;
			}(std::tuple<attribute_type...>{}),
			"vertex_program must be invocable"
		);

		using vertex_program_res_type = decltype( //
			vertex_program( //
				std::declval<r4::vector4<real>>(),
				std::declval<attribute_type>()...
			)
		);

		static_assert(
			utki::is_specialization_of_v<std::tuple, vertex_program_res_type>,
			"vertex program return type must be std::tuple"
		);

		static_assert(
			std::is_same_v<r4::vector4<real>, std::tuple_element_t<0, vertex_program_res_type>>,
			"first element of vertex program return tuple must be r4::vector4<real>"
		);

		for (const auto& unprocessed_face : mesh.faces) {
			std::array<vertex_program_res_type, 3> face{
				std::apply(vertex_program, mesh.vertices[unprocessed_face[0]]),
				std::apply(vertex_program, mesh.vertices[unprocessed_face[1]]),
				std::apply(vertex_program, mesh.vertices[unprocessed_face[2]])
			};

			auto bb_segment =
				calc_bounding_box_segment(std::get<0>(face[0]), std::get<0>(face[1]), std::get<0>(face[2]));

			r4::rectangle<real> bounding_box = {
				bb_segment.p1,
				bb_segment.p2 - bb_segment.p1 + decltype(bb_segment.p1)(1) // +1 because segment is [p1, p2]
			};

			auto framebuffer_span = ctx.get_framebuffer().span().subspan(bounding_box.to<unsigned>());

			auto p = bounding_box.p;
			for (auto line : framebuffer_span) {
				for (auto& framebuffer_pixel : line) {
					r4::vector2<real> v0 = std::get<0>(face[0]);
					r4::vector2<real> v1 = std::get<0>(face[1]);
					r4::vector2<real> v2 = std::get<0>(face[2]);

					auto edge_0_1 = v1 - v0;
					auto edge_1_2 = v2 - v1;
					auto edge_2_0 = v0 - v2;

					auto barycentric = r4::vector3<real>{
						edge_function(edge_1_2, p - v1),
						edge_function(edge_2_0, p - v2),
						edge_function(edge_0_1, p - v0)
					};

					if (barycentric.is_positive_or_zero()) {
						// pixel is inside of the face triangle

						// normalize barycentric coordinates
						auto triangle_area = edge_0_1.cross(edge_2_0);
						barycentric /= triangle_area;

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
							"fragment_program must be invocable"
						);

						auto pixel_color = std::apply(fragment_program, interpolated_attributes);

						using framebuffer_pixel_value_type =
							std::remove_reference_t<decltype(framebuffer_pixel)>::value_type;

						framebuffer_pixel = rasterimage::to<framebuffer_pixel_value_type>(pixel_color);
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