#pragma once

#include <rasterimage/image.hpp>

namespace cpugl {

template <typename image_type>
class texture
{
	// NOTE: utki::is_specialization_of_v doesn't work for non-type templates
	// static_assert(
	// 	utki::is_specialization_of_v<rasterimage::image, image_type>,
	// 	"image_type must be a specialization of rasterimage::image"
	// );

	const image_type& image;
	r4::vector2<real> dims;

public:
	texture(const image_type& image) :
		image(image),
		dims(image.dims().template to<real>())
	{}

	const image_type::pixel_type& get(const r4::vector2<real>& tex_coords) const
	{
		// TODO:
		return image[0][0];
	}
};

template <typename image_type>
texture<image_type> make_texture(const image_type& image){
	return texture<image_type>(image);
}

} // namespace cpugl
