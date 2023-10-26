#pragma once

#include "../context.hpp"
#include "../mesh.hpp"
#include "../texture.hpp"

namespace cpugl {

class texture_pos_tex_shader
{
public:
	template <typename image_type>
	static void render( //
		context& ctx,
		const r4::matrix4<real>& matrix,
		const texture<image_type>& tex,
		const mesh<tex_coord_type>& mesh
	)
	{
		// TODO:
	}
};

} // namespace cpugl
