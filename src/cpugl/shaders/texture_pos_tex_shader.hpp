#pragma once

#include "../context.hpp"
#include "../mesh.hpp"

namespace cpugl{

class texture_pos_tex_shader{
public:
    static void render( //
		context& ctx,
		const r4::matrix4<real>& matrix,
        // TODO:
		const mesh<r4::vector2<cpugl::real>>& mesh
	);
};

}
