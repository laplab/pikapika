CXX = g++

# modify these paths to point to your local sdl install.
SDL_PATH_LIB = /usr/local/lib
SDL_PATH_INC = /usr/local/include

SDL_LIB = -L$(SDL_PATH_LIB) -lSDL2
SDL_INCLUDE = -I$(SDL_PATH_LIB)

LUA_LIB =  -Lsrc/z8lua -lluaz8
UTF8_UTIL_BASE = src/utf8-util/utf8-util

DEFINES = -DTAC08_PLATFORM=PLATFORM_DESKTOP_LINUX

CXXFLAGS_DEBUG = -DDEBUG -ggdb -Wall -c -std=c++11 $(SDL_INCLUDE) -I$(UTF8_UTIL_BASE) $(DEFINES)
CXXFLAGS_RELEASE = -O3 -ggdb -Wall -c -std=c++11 $(SDL_INCLUDE) -I$(UTF8_UTIL_BASE) $(DEFINES)

CXXFLAGS = $(CXXFLAGS_RELEASE)

LDFLAGS = $(SDL_LIB) $(LUA_LIB)
EXE = tac08

all: $(EXE)

$(EXE): bin/main.o bin/hal_core.o bin/hal_palette.o bin/pico.a
	$(CXX) $^ $(LDFLAGS) -o $@
	objdump -t -C $@ | sort >bin/app.symbols
	@echo "Built All The Things!!!"

bin/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/hal_core.o: src/hal_core.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/hal_palette.o: src/hal_palette.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/pico_core.o: src/pico_core.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/pico_gfx.o: src/pico_gfx.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/pico_data.o: src/pico_data.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/pico_memory.o: src/pico_memory.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/pico_cart.o: src/pico_cart.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/pico_script.o: src/pico_script.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/utils.o: src/utils.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/utf8-util.o: $(UTF8_UTIL_BASE)/utf8-util.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

bin/pico.a: bin/pico_core.o bin/pico_gfx.o bin/pico_data.o bin/pico_memory.o bin/pico_cart.o bin/pico_script.o bin/utils.o bin/utf8-util.o
	ar rcs $@ $^

clean:
	@rm bin/*.o || true
	@rm $(EXE) || true

run: all
	./$(EXE)

copy: all
	@cp tac08 ~/bin/ || true
