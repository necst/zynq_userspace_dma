
lib_dmabuf_dir := ../../lib_dmabuf
headers = $(wildcard *.h)

test_sources = $(wildcard test_*.c)
# test_objects = $(patsubst %.c,%.o,$(test_sources))
test_targets = $(patsubst %.c,%,$(test_sources))

utils_sources = $(wildcard utils*.c)
utils_objects = $(patsubst %.c,%.o,$(utils_sources))

CFLAGS += -Wall -Wextra -pedantic -std=gnu99 -I $(lib_dmabuf_dir)
LDFLAGS =

dma_name = dmabuf
dma_static_lib = $(lib_dmabuf_dir)/lib$(dma_name).a

utils_name = utils
utils_lib = lib$(utils_name).a

.PHONY: clean all static tests_all
.PRECIOUS: %.o

all: tests_all

%.o: %.c $(headers)
	$(CC) -c $< $(CFLAGS)

static_lib:
	$(MAKE) -C $(lib_dmabuf_dir) static

$(utils_lib): $(utils_objects)
	$(AR) rcs $@ $^

test_%: test_%.o $(utils_lib) static_lib
	$(CC) -static $< -L$(lib_dmabuf_dir) -L. -l$(dma_name) -l$(utils_name) -o $@

tests_all: $(test_targets)

clean:
	@rm -rf *.o 2> /dev/null

distclean: clean
	@rm -rf $(test_targets) 2> /dev/null

docs:
	doxygen $(lib_dmabuf_dir)/Doxyfile

