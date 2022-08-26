#ifndef PICO_CORE_H
#define PICO_CORE_H

#include <utility>

#include "pico_gfx.h"

extern "C" {
  void hello_laplab();
}

namespace pico_api {
	void update_fps(uint32_t target, uint32_t actual, uint32_t sys, uint32_t cpu);
	void update_display_area(uint32_t x, uint32_t y);

	void set_time(uint32_t value);
	uint32_t get_time();

	void load(std::string cartname);
	void run();

	uint8_t peek(uint16_t a);
	uint16_t peek2(uint16_t a);
	uint32_t peek4(uint16_t a);

	void poke(uint16_t a, uint8_t v);
	void poke2(uint16_t a, uint16_t v);
	void poke4(uint16_t a, uint32_t v);

	void memory_set(uint16_t a, uint8_t val, uint16_t len);
	void memory_cpy(uint16_t dest_a, uint16_t src_a, uint16_t len);

	uint32_t dget(uint16_t a);
	void dset(uint16_t a, uint32_t v);

	int btn();
	int btn(int n, int player = 0);

	int btnp();
	int btnp(int n, int player = 0);

	int stat(int key, std::string& sval, int& ival, double& fval);

	void reload(uint16_t dest_addr, uint16_t source_addr, uint16_t len);

}  // namespace pico_api

// extended api
namespace pico_apix {
	void screen(uint16_t w, uint16_t h);
	void menu();

	void sprites();
	void sprites(int page);

	void maps();
	void maps(int page);

	void fonts();
	void fonts(int page);

	std::pair<std::string, bool> dbg_getsrc(std::string src, int line);
	int dbg_getsrclines();
}  // namespace pico_apix

namespace pico_control {
	void init();
	void frame_start();
	void frame_end();
	pico_api::colour_t* get_buffer(int& width, int& height);
	void set_sprite_data_4bit(std::string data);
	void set_sprite_data_8bit(std::string data);
	void set_sprite_flags(std::string flags);
	void set_map_data(std::string data);
	void set_font_data(std::string data);
	void set_input_state(int state, int player = 0);
	void copy_shared_data();
	void test_integrity();
	void begin_pause_menu();
	bool is_pause_menu();
	void end_pause_menu();
	uint8_t* get_music_data();
	uint8_t* get_sfx_data();
	void restartCart();
	void init_rom();

	void displayerror(const std::string& msg);

}  // namespace pico_control

#endif /* PICO_CORE_H */