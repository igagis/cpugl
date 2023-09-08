include prorab.mk
include prorab-test.mk

this_name := app

$(eval $(call prorab-config, ../../config))

this_srcs += $(call prorab-src-dir, src)

# this_ldlibs += ../../src/out/$(c)/libsvgren$(dot_so)
this_ldlibs += -lutki
this_ldflags += -L../../src

ifeq ($(os), macosx)
else ifeq ($(os),windows)
else
    this_ldlibs += -lX11
endif

this_no_install := true

$(eval $(prorab-build-app))

# ifeq ($(os),windows)
#     this_run_cmd := $(a)cp $(d)../../src/out/$(c)/libsvgren$(dot_so) $(d) && $$^)
# else
    # this_run_cmd := $(a)(cd $(d); LD_LIBRARY_PATH=../../src/out/$(c) $$^)
# endif

this_run_name := $(this_name)
this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c)
$(eval $(prorab-run))

# define this_rules
#     run:: $(prorab_this_name)
# $(.RECIPEPREFIX)@echo running $$^...
# $(.RECIPEPREFIX)$(a)cp $(d)../../src/out/$(c)/libsvgren$(dot_so) . || true
# $(.RECIPEPREFIX)$(a)LD_LIBRARY_PATH=$(d)../../src/out/$(c) $(d)out/$(c)/tests $(d)../samples/testdata/tiger.svg $(d)out.png
# endef
# $(eval $(this_rules))

# add dependency on libcpugl
# $(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libsvgren$(dot_so))

$(eval $(call prorab-include, ../../src/makefile))
