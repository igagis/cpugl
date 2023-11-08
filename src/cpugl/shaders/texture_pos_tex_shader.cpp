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

#include "texture_pos_tex_shader.hpp"

#include "../pipeline.hpp"
#include "../texture.hpp"

using namespace cpugl;

void texture_pos_tex_shader::render( //
	context& ctx,
	const r4::matrix4<real>& matrix,
	const rasterimage::image_variant& tex,
	const mesh<tex_coord_type>& mesh
)
{
	std::visit(
		[&ctx, &matrix, &mesh](const auto& image) {
			if constexpr (std::is_same_v<uint8_t, typename std::remove_reference_t<decltype(image)>::value_type>) {
				auto tex = make_texture(image);

				pipeline::render<false>(
					ctx,
					[&matrix](const r4::vector3<real>& pos, const r4::vector2<real> tex_coord) {
						return std::make_tuple(matrix * pos, tex_coord);
					},
					[&tex](const r4::vector2<real>& tex_coord) {
						return rasterimage::get_rgba(tex.get(tex_coord));
					},
					mesh
				);
			} else {
				std::cout << "texture_pos_tex_shader::render(): non-uint8_t textures are not supported" << std::endl;
			}
		},
		tex.variant
	);
}
