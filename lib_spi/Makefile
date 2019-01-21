
headers = $(wildcard *.h)

spi_sources = ./xspi.c
spi_objects = $(patsubst %.c,%.o,$(spi_sources))
modpath := $(shell cd $(CURDIR)/../spibuf/ && pwd)

CFLAGS += -Wall -Wextra -pedantic -std=c99 -D MODPATH=\"$(modpath)\" 
LDFLAGS =

spi_name = spibuf
spi_static_lib = lib$(spi_name).a
spi_dynamic_lib = lib$(spi_name).so

.PHONY: clean all static dynamic
.PRECIOUS: %.o

all: static

%.o: %.c $(headers)
	$(CC) -c $< $(CFLAGS)

$(spi_static_lib): $(spi_objects)
	$(AR) rcs $@ $^

static: $(spi_static_lib)

$(spi_dynamic_lib): CFLAGS += -fPIC
$(spi_dynamic_lib): $(spi_objects)
	$(CC) $(LDFLAGS) -shared $^ -o $@
	@echo "run application with LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:"$(shell pwd)

dynamic: $(spi_dynamic_lib)

clean:
	@rm -rf *.o 2> /dev/null

distclean: clean
	@rm -rf $(spi_static_lib) $(spi_dynamic_lib) 2> /dev/null

docs:
	doxygen Doxyfile
test: static
	gcc test_dc.c libspibuf.a -o exefile

