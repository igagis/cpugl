#include <chrono>

#include <utki/debug.hpp>
#include <utki/config.hpp>
#include <utki/time.hpp>

#include <papki/fs_file.hpp>

#include <cpugl/shaders/clr_pos_shader.hpp>

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
#ifdef DEBUG
	// auto loadStart = utki::get_ticks_ms();
#endif
	
	// auto dom = svgdom::load(papki::fs_file(filename));
	// utki::assert(dom, SL);
	
	// LOG([&](auto&o){o << "SVG loaded in " << float(utki::get_ticks_ms() - loadStart) / 1000.0f << " sec." << std::endl;})
	
	// auto render_start_ms = utki::get_ticks_ms();
	
	// auto image = rasterimage::image_variant(svgren::rasterize(*dom));

	// const auto& img = image.get<rasterimage::format::rgba>();
	
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
	// utki::log([&](auto&o){o << "SVG rendered in " << float(utki::get_ticks_ms() - render_start_ms) / 1000.0f << " sec." << std::endl;});
	
	// utki::log([&](auto&o){o << "img.dims = " << img.dims() << " img.pixels.size() = " << img.pixels().size() << std::endl;});

	// image.write_png(papki::fs_file(out_filename));
	
#if M_OS == M_OS_LINUX
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
			case Expose:
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

					const std::vector<r4::vector4<cpugl::real>> vertices = {
						{10, 10, 0, 1}, // NOLINT
						{10, 500, 0, 1}, // NOLINT
						{500, 10, 0, 1} // NOLINT
					};

					const std::vector<r4::vector4<float>> colors = {
						{1, 0, 0, 1},
						{0, 1, 0, 1},
						{0, 0, 1, 1}
					};

					std::vector<unsigned> indices = {0, 1, 2};

					auto vao = cpugl::make_vertex_array(
						cpugl::rendering_mode::triangles,
						std::move(indices),
						utki::make_span(vertices),
						utki::make_span(colors)
					);

					cpugl::clr_pos_shader shader;

					shader.render(
						glc,
						r4::matrix4<cpugl::real>().set_identity().translate(10, 0, 0), // NOLINT
						vertices,
						colors
					);

					fb.span().swap_red_blue();

					// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
					auto ximage = XCreateImage(display, visual, utki::byte_bits * 3, ZPixmap, 0, reinterpret_cast<char*>(fb.pixels().data()), fb.dims().x(), fb.dims().y(), utki::byte_bits, 0);
					utki::scope_exit scope_exit([ximage](){
						ximage->data = nullptr; // Xlib will try to deallocate data which is owned by 'img' variable.
						XDestroyImage(ximage);
					});
					
					// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast, cppcoreguidelines-pro-bounds-pointer-arithmetic)
					XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 1, 1, fb.dims().x(), fb.dims().y());
				}
				break;
			case KeyPress:
			case ButtonPress:
				exit(0);
				break;
		}
	}
#endif

	utki::log([](auto&o){o << "[PASSED]" << std::endl;});
}
