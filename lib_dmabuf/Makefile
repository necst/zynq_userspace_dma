
headers = $(wildcard *.h)

dma_sources = $(wildcard dma*.c)
dma_objects = $(patsubst %.c,%.o,$(dma_sources))

test_sources = $(wildcard test_*.c)
test_objects = $(patsubst %.c,%.o,$(test_sources))
test_targets = $(patsubst %.c,%,$(test_sources))

utils_sources = $(wildcard utils*.c)
utils_objects = $(patsubst %.c,%.o,$(utils_sources))

CFLAG += -Wall -Wextra -pedantic -std=gnu89

dma_name = dmabuf
dma_static_lib = lib$(dma_name).a

utils_name = utils
utils_lib = lib$(utils_name).a

.PHONY: clean static

%.o: %.c $(headers)
	$(CC) -c $< $(CFLAGS)

$(dma_static_lib): $(dma_objects)
	$(AR) rcs $@ $^

static: $(static_lib)

$(utils_lib): $(utils_objects)
	$(AR) rcs $@ $^

test_%: test_%.o $(dma_static_lib) $(utils_lib)
	$(CC) -static $< -L. -l$(dma_name) -l$(utils_name) -o $@

clean:
	@rm -rf *.o 2> /dev/null

distclean: clean
	@rm -rf $(test_targets) *.a 2> /dev/null

