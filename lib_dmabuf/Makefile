
headers = $(widlcard *.h)
dma_sources = $(wildcard dma*.c)
dma_objects = $(patsubst %.c,%.o,$(dma_sources))
CFLAGS += -Wall -pedantic
static_lib = libdmabuf.a

%.o: %.c $(headers)
	$(CC) -c $< $(CFLAGS)

$(static_lib): $(dma_objects)
	$(AR) rcs $@ $^

static: $(static_lib)

test: test.c $(static_lib)
	$(CC) -static $< -L. -ldmabuf -o $@

clean:
	@rm -rf $(static_lib) *.o 2> /dev/null

