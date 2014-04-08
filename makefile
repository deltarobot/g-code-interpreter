objects = main configure block cnc
main_incl = configure block cnc
configure_incl = configure
block_incl = block configure
cnc_incl = block cnc comm configure

cc=gcc -Isrc/include/ -Ilib/ -Wall -Wextra -pedantic -lm -o

code = $(patsubst %,src/main/%.c,$1)
incl = $(patsubst %,src/include/%.h,$1)
lib = $(patsubst %,lib/%.o,$1)
targ = $(patsubst %,target/%.o,$1)
test = $(patsubst %,src/test/%_test.c,$1)
testexe = $(patsubst %,target/%_test,$1)

objects_no_main = $(filter-out main,$(objects))
all_libs = $(foreach object,$(objects),$($(object)_lib))

define make_object =
$(call targ,$1): $(call code,$1) $(call incl,$($1_incl)) | target
	$(cc) $(call targ,$1) -c $(call code,$1)
endef

define make_lib =
$(call lib,$1): lib/$1.c lib/$1.h
	$(cc) $(call lib,$1) -c lib/$1.c
endef

define make_test =
$(call testexe,$1): $(call test,$1) $(call code,$1) $(call incl,$($1_incl)) $(call lib,$($1_lib) CuTest)
	$(cc) $(call testexe,$1) -Isrc/main/ -DTEST $(call test,$1) $(call lib,$($1_lib) CuTest)
	target/$1_test; if [ $$$$? -ne 0 ]; then rm target/$1_test; exit 1; fi
endef

all: $(call targ,$(objects)) $(call testexe,$(objects_no_main)) $(call lib,$(all_libs))
	$(cc) target/g-code-interpreter $(call targ,$(objects)) $(call lib,$(all_libs))

$(foreach object,$(objects),$(eval $(call make_object,$(object))))

$(foreach object,$(objects_no_main),$(eval $(call make_test,$(object))))

$(foreach lib,$(all_libs) CuTest,$(eval $(call make_lib,$(lib))))

install: all | ~/bin
	cp target/g-code-interpreter ~/bin/

~/bin:
	mkdir ~/bin

target:
	mkdir target

clean:
	rm -r target
