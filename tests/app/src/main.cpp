#include <chrono>

#include <utki/debug.hpp>
#include <utki/config.hpp>
#include <utki/time.hpp>

#include <papki/fs_file.hpp>

#include <cpugl/shaders/pos_clr_shader.hpp>

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

					constexpr auto l = 1;
					constexpr auto t = 1;
					constexpr auto r = 799;
					constexpr auto b = 599;

					const std::vector<r4::vector4<cpugl::real>> vertices = {
						{l, t, 0, 1},
						{l, b, 0, 1},
						{r, b, 0, 1},
						{r, t, 0, 1}
					};

					const std::vector<r4::vector4<cpugl::real>> colors = {
						{1, 0, 0, 1},
						{0, 1, 0, 1},
						{0, 0, 1, 1},
						{0, 1, 1, 1},
					};

					std::vector<std::array<unsigned, 3>> faces = {
						{0, 1, 3},
						{3, 1, 2}
					};

					auto vao = cpugl::make_mesh(
						std::move(faces),
						utki::make_span(vertices),
						utki::make_span(colors)
					);

					cpugl::pos_clr_shader shader;

					shader.render(
						glc,
						r4::matrix4<cpugl::real>().set_identity(),
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
				break;
			case KeyPress:
			case ButtonPress:
				exit(0);
				break;
		}
	}
#endif
}
