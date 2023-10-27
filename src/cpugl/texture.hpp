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
		ASSERT(tex_coords.is_positive_or_zero())
		ASSERT(tex_coords.x() <= 1 && tex_coords.y() <= 1)
		auto tc = this->dims.comp_mul(tex_coords).to<unsigned>();
		return image[tc.y()][tc.x()];
	}
};

template <typename image_type>
texture<image_type> make_texture(const image_type& image){
	return texture<image_type>(image);
}

} // namespace cpugl
