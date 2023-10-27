#include <tst/set.hpp>
#include <tst/check.hpp>

#include <cpugl/texture.hpp>

namespace{
const tst::set set("texture", [](tst::suite& suite){
    suite.add("tex_coord_one_does_not_cause_buffer_overflow", [](){
        rasterimage::image<uint8_t, 1> im{2, 2};

        im[0][0] = 0x1;
        im[0][1] = 0x2;
        im[1][0] = 0x3;
        im[1][1] = 0x4;

        auto tex = cpugl::make_texture(im);

        tst::check_eq(tex.get({0, 0})[0], im[0][0][0], SL);
        tst::check_eq(tex.get({1, 0})[0], im[0][1][0], SL);
        tst::check_eq(tex.get({0, 1})[0], im[1][0][0], SL);
        tst::check_eq(tex.get({1, 1})[0], im[1][1][0], SL);
    });
});
}
