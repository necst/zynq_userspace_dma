
headers = $(wildcard *.h)

dma_sources = $(wildcard dma*.c)
dma_objects = $(patsubst %.c,%.o,$(dma_sources))

CFLAGS += -Wall -Wextra -pedantic -std=gnu99
LDFLAGS =

dma_name = dmabuf
dma_static_lib = lib$(dma_name).a
dma_dynamic_lib = lib$(dma_name).so

.PHONY: clean all static dynamic
.PRECIOUS: %.o

all: static

%.o: %.c $(headers)
	$(CC) -c $< $(CFLAGS)

$(dma_static_lib): $(dma_objects)
	$(AR) rcs $@ $^

static: $(dma_static_lib)

$(dma_dynamic_lib): CFLAGS += -fPIC
$(dma_dynamic_lib): $(dma_objects)
	$(CC) $(LDFLAGS) -shared $^ -o $@
	@echo "run application with LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:"$(shell pwd)

dynamic: $(dma_dynamic_lib)

clean:
	@rm -rf *.o 2> /dev/null

distclean: clean
	@rm -rf $(test_targets) $(dma_static_lib) $(dma_dynamic_lib) 2> /dev/null

docs:
	doxygen Doxyfile

