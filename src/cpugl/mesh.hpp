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

#include <numeric>
#include <vector>

#include <utki/debug.hpp>
#include <utki/span.hpp>

namespace cpugl {

template <typename... attribute_type>
class mesh
{
public:
	std::vector<std::tuple<attribute_type...>> vertices;

	std::vector<r4::vector3<unsigned>> faces;
};

template <typename... attribute_type>
mesh<attribute_type...> make_mesh(
	std::vector<r4::vector3<unsigned>> faces,
	utki::span<const attribute_type>... attribute
)
{
	// all spans must be of the same size
	ASSERT((... == attribute.size()))

	auto attrs_tuple = std::make_tuple(attribute...);

	mesh<attribute_type...> vao;

	vao.faces = std::move(faces);

	for (auto iters = std::make_tuple(attribute.begin()...); //
		 std::get<0>(iters) != std::get<0>(attrs_tuple).end();
		 std::apply(
			 [](auto&... i) {
				 (..., ++i);
			 },
			 iters
		 ))
	{
		vao.vertices.push_back(std::apply(
			[](auto... i) {
				return std::make_tuple(*i...);
			},
			iters
		));
	}

	// assert that all the indices are within vertices array
	ASSERT(std::accumulate( //
		vao.faces.begin(),
		vao.faces.end(),
		true,
		[&vao](auto acc, auto face) {
			return acc
				&& std::accumulate( //
					   face.begin(),
					   face.end(),
					   true,
					   [&vao](auto acc, auto val) {
						   return acc && (val < vao.vertices.size());
					   }
				);
		}
	))

	return vao;
}

} // namespace cpugl
