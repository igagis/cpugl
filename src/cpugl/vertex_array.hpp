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

#include <vector>
#include <numeric>

#include <utki/debug.hpp>
#include <utki/span.hpp>

namespace cpugl {

/**
 * @brief Vertex data rendering mode.
 * Enumeration defining how to interpret vertex data when rendering.
 */
enum class rendering_mode {
	triangles,
	triangle_fan,
	triangle_strip,

	enum_size
};

template <typename... attribute_type>
class vertex_array
{
public:
	std::vector<std::tuple<attribute_type...>> vertices;

	std::vector<unsigned> indices;

	rendering_mode mode;
};

template <typename... attribute_type>
vertex_array<attribute_type...> make_vertex_array(
	utki::span<const attribute_type>... attribute,
	std::vector<unsigned> indices,
	rendering_mode mode
)
{
	// all spans must be of the same size
	ASSERT((... == attribute.size()))

	auto attrs_tuple = std::make_tuple(attribute...);

	vertex_array<attribute_type...> vao;

	vao.indices = std::move(indices);
	vao.mode = mode;

	for (auto iters = std::make_tuple(attribute.begin()...); //
		 std::get<0>(iters) != std::get<0>(attrs_tuple).end();
		 std::apply(
			 [](auto&... i) {
				 (..., ++i);
			 },
			 iters
		 ))
	{
		vao.push_back(std::apply(
			[](auto... i) {
				return std::make_tuple(*i...);
			},
			iters
		));
	}

    // assert that all the indices are within vertices array
	ASSERT(std::accumulate( //
		vao.indices.begin(),
		vao.indices.end(),
		true,
		[&vao](auto acc, auto val) {
			return acc && (val < vao.vertices.size());
		}
	))

	return vao;
}

} // namespace cpugl
