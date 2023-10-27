#pragma once

#include <rasterimage/image_variant.hpp>

#include "../context.hpp"
#include "../mesh.hpp"
#include "../texture.hpp"

namespace cpugl {

class texture_pos_tex_shader
{
public:
	static void render( //
		context& ctx,
		const r4::matrix4<real>& matrix,
		const rasterimage::image_variant& tex,
		const mesh<tex_coord_type>& mesh
	);
};

} // namespace cpugl
