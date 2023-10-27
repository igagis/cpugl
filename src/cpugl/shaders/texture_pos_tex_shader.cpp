#include "texture_pos_tex_shader.hpp"

#include "../texture.hpp"
#include "../pipeline.hpp"

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
                    [&matrix](const r4::vector4<real>& pos, const r4::vector2<real> tex_coord){
                        return std::make_tuple(matrix * pos, tex_coord);
                    },
                    [&tex](const r4::vector2<real>& tex_coord){
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
