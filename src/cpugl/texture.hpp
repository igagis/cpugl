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

#include <rasterimage/image.hpp>

#include "config.hpp"

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
		auto tc = this->dims.comp_mul(tex_coords).template to<unsigned>();
		if (tc.x() == this->image.dims().x()) {
			--tc.x();
		}
		if (tc.y() == this->image.dims().y()) {
			--tc.y();
		}
		ASSERT(tc.x() < this->image.dims().x())
		ASSERT(tc.y() < this->image.dims().y())
		return this->image[tc.y()][tc.x()];
	}
};

template <typename image_type>
texture<image_type> make_texture(const image_type& image)
{
	return texture<image_type>(image);
}

} // namespace cpugl
