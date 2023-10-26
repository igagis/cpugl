#pragma once

#include <rasterimage/image.hpp>

namespace cpugl {

template <typename image_type>
class texture
{
	static_assert(
		utki::is_specialization_of_v<rasterimage::image, image_type>,
		"image_type must be a specialization of rasterimage::image"
	);

	image_type image;
	r4::vector2<cpugl::real> dims;

public:
	texture(image_type image) :
		image(std::move(image))
	{}

	const image_type::pixel_type& get(const r4::vector2<cpugl::real>& tex_coords) const
	{
		// TODO:
	}
};

} // namespace cpugl
