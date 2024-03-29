#include <chrono>

#include <utki/debug.hpp>
#include <utki/config.hpp>
#include <utki/time.hpp>

#include <papki/fs_file.hpp>

#include <rasterimage/image_variant.hpp>

#include <cpugl/shaders/pos_clr_shader.hpp>
#include <cpugl/shaders/color_pos_shader.hpp>
#include <cpugl/shaders/texture_pos_tex_shader.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if M_OS == M_OS_LINUX
#	include <X11/Xlib.h>
#	include <X11/Xutil.h>
#endif

#ifdef assert
#	undef assert
#endif

// NOLINTNEXTLINE(bugprone-exception-escape): fatal exceptions are not caught
int main(int argc, char **argv){
	r4::vector3<cpugl::real> position{0, 0, 0};
	auto rotation = r4::quaternion<cpugl::real>().set_identity();

	constexpr auto l = -1;
	constexpr auto t = -1;
	constexpr auto r = 1;
	constexpr auto b = 1;
	constexpr auto d = 1;

	auto tex = rasterimage::read_jpeg(papki::fs_file("texture.jpg"));

	const std::vector<r4::vector3<cpugl::real>> vertices = {
		// front
		{l, t, -d},
		{l, b, -d},
		{r, b, -d},
		{r, t, -d},

		// back
		{l, t, d},
		{l, b, d},
		{r, b, d},
		{r, t, d},

		// left
		{l, t, -d},
		{l, b, -d},
		{l, b, d},
		{l, t, d},

		// right
		{r, t, -d},
		{r, b, -d},
		{r, b, d},
		{r, t, d},

		// top
		{l, t, -d},
		{l, t, d},
		{r, t, d},
		{r, t, -d},
	};

	std::vector<std::array<unsigned, 3>> faces = {
		{0, 1, 3},
		{3, 1, 2},

		{7, 5, 4},
		{6, 5, 7},

		{11, 9, 8},
		{10, 9, 11},

		{12, 13, 15},
		{15, 13, 14},

		{19, 17, 16},
		{18, 17, 19},
	};

	const std::vector<r4::vector2<cpugl::real>> tex_coords = {
		{0, 0},
		{0, 1},
		{1, 1},
		{1, 0},

		{0, 0},
		{0, 1},
		{1, 1},
		{1, 0},

		{0, 0},
		{0, 1},
		{1, 1},
		{1, 0},

		{0, 0},
		{0, 1},
		{1, 1},
		{1, 0},

		{0, 0},
		{0, 1},
		{1, 1},
		{1, 0},
	};

	const std::vector<r4::vector4<cpugl::real>> colors = {
		{1, 0, 0, 1},
		{0, 1, 0, 1},
		{0, 0, 1, 1},
		{0, 1, 1, 1},
	};

	auto vao = cpugl::make_mesh(
		std::move(faces),
		utki::make_span(vertices)
		, utki::make_span(tex_coords)
		// ,utki::make_span(colors)
	);

	constexpr auto width = 800;
	constexpr auto height = 600;

	Display *display = XOpenDisplay(nullptr);
	
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast, cppcoreguidelines-pro-bounds-pointer-arithmetic)
	Visual *visual = DefaultVisual(display, 0);
	
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast, cppcoreguidelines-pro-bounds-pointer-arithmetic)
	Window window = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, width, height, 1, 0, 0);
	
	if(visual->c_class != TrueColor){
		utki::log([](auto&o){o << "Cannot handle non true color visual ...\n" << std::endl;});
		return 1;
	}	
	
	XSelectInput(display, window, ButtonPressMask|ExposureMask|KeyPressMask);
	
	XMapWindow(display, window);
	
	while(true){
		XEvent ev;
		XNextEvent(display, &ev);
		switch(ev.type){
			default:
				break;
			// case Expose:
			// 	// std::cout << "Expose" << std::endl;
			// 	break;
			case KeyPress:
				{
					constexpr auto esc_key = 9;
					constexpr auto cursor_left_key = 113;
					constexpr auto cursor_right_key = 114;
					constexpr auto cursor_up_key = 111;
					constexpr auto cursor_down_key = 116;
					constexpr auto w_key = 25;
					constexpr auto a_key = 38;
					constexpr auto s_key = 39;
					constexpr auto d_key = 40;
					constexpr auto page_up_key = 112;
					constexpr auto page_down_key = 117;

					std::cout << "keycode = " << ev.xkey.keycode << std::endl;

					constexpr auto translate_step = 0.1;
					constexpr auto rotation_step_rad = utki::pi / 30;

					switch(ev.xkey.keycode){
						default:
							break;
						case esc_key:
							exit(0);
							break;
						case cursor_left_key:
							rotation =
								r4::quaternion<cpugl::real>(
									r4::vector3<cpugl::real>(0, 1, 0) * rotation_step_rad
								) * rotation;
							break;
						case cursor_right_key:
							rotation =
								r4::quaternion<cpugl::real>(
									r4::vector3<cpugl::real>(0, -1, 0) * rotation_step_rad
								) * rotation;
							break;
						case cursor_up_key:
							rotation =
								r4::quaternion<cpugl::real>(
									r4::vector3<cpugl::real>(-1, 0, 0) * rotation_step_rad
								) * rotation;
							break;
						case cursor_down_key:
							rotation =
								r4::quaternion<cpugl::real>(
									r4::vector3<cpugl::real>(1, 0, 0) * rotation_step_rad
								) * rotation;
							break;
						case w_key:
							position.y() -= translate_step;
							break;
						case a_key:
							position.x() -= translate_step;
							break;
						case s_key:
							position.y() += translate_step;
							break;
						case d_key:
							position.x() += translate_step;
							break;
						case page_up_key:
							position.z() += translate_step;
							break;
						case page_down_key:
							position.z() -= translate_step;
							break;
					}
				}
				// XClearArea(display, window, 0, 0, width, height, True);
				break;
			case ButtonPress:
				exit(0);
				break;
		}

		// render
		{
			int dummy_int = 0;
			unsigned dummy_unsigned = 0;
			Window dummy_window = 0;
			r4::vector2<unsigned> win_dims;
			
			XGetGeometry(
				display,
				window,
				&dummy_window,
				&dummy_int,
				&dummy_int,
				&win_dims.x(),
				&win_dims.y(),
				&dummy_unsigned,
				&dummy_unsigned
			);
			
			cpugl::context glc;

			cpugl::context::fb_image_type fb(win_dims);

			glc.set_framebuffer(fb);

			constexpr auto bg_color = decltype(fb)::pixel_type{0, 0, 0, 0xff};
			glc.clear(bg_color);

			r4::matrix4<cpugl::real> matrix;
			matrix.set_identity();

			matrix.scale(cpugl::real(width) / 2, cpugl::real(height) / 2);
			matrix.translate(1, 1, 0);

			// matrix.scale(1, -1);
			// matrix.rotate(r4::quaternion<cpugl::real>({utki::pi, 0, 0}));
			// matrix.frustum(-2, 2, -1.5, 1.5, 2, 100);
			// matrix.translate(0, 0, 1); // move projection plane to (0, 0, 0)
			// matrix.scale(1, -1);

			matrix.scale(1, 4/3.0);
			matrix.perspective();
			
			matrix.translate(0, 0, 4);

			matrix.translate(position);
			matrix.rotate(rotation);

			cpugl::texture_pos_tex_shader::render(
				glc,
				matrix,
				tex,
				vao
			);

			fb.span().swap_red_blue();

			// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
			auto ximage = XCreateImage(display, visual, utki::byte_bits * 3, ZPixmap, 0, reinterpret_cast<char*>(fb.pixels().data()), fb.dims().x(), fb.dims().y(), utki::byte_bits, 0);
			utki::scope_exit scope_exit([ximage](){
				ximage->data = nullptr; // Xlib will try to deallocate data which is owned by 'img' variable.
				XDestroyImage(ximage);
			});
			
			// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast, cppcoreguidelines-pro-bounds-pointer-arithmetic)
			XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, fb.dims().x(), fb.dims().y());
		}
	}
}
