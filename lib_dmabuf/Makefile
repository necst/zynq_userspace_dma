
headers = $(wildcard *.h)
dma_sources = $(wildcard dma*.c)
dma_objects = $(patsubst %.c,%.o,$(dma_sources))
test_sources = $(wildcard test_*.c)
test_objects = $(patsubst %.c,%.o,$(test_sources))
CFLAGS += -Wall -pedantic
static_lib = libdmabuf.a

%.o: %.c $(headers)
	$(CC) -c $< $(CFLAGS)

$(static_lib): $(dma_objects)
	$(AR) rcs $@ $^

static: $(static_lib)


test_%: test_%.o $(static_lib)
	$(CC) -static $< -L. -ldmabuf -o $@

clean:
	@rm -rf $(static_lib) *.o 2> /dev/null

