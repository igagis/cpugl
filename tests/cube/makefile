include prorab.mk
include prorab-test.mk

this_name := cube

$(eval $(call prorab-config, ../../config))

this_srcs += $(call prorab-src-dir, src)

this_cxxflags += -I ../../src

this_cpugl_lib := ../../src/out/$(c)/libcpugl$(dot_so)

this_ldlibs += -lutki $(this_cpugl_lib) -lm

ifeq ($(os), macosx)
else ifeq ($(os),windows)
else
    this_ldlibs += -lX11
endif

this_no_install := true

$(eval $(prorab-build-app))

this_run_name := $(this_name)
this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name) $(this_cpugl_lib)
this_test_ld_path := ../../src/out/$(c)
$(eval $(prorab-run))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_cpugl_lib)))

$(eval $(call prorab-include, ../../src/makefile))
