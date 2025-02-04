#include <SDL2/SDL.h>

#include "config.h"
#include "hal_core.h"
#include "pico_cart.h"
#include "pico_core.h"
#include "pico_data.h"
#include "pico_script.h"
#include "utils.h"

void load_cart(std::string filename) {
	path::test();
	filename = path::normalisePath(filename);
	std::string data = FILE_LoadFile(filename);
	if (data.size() == 0) {
		throw std::runtime_error(std::string("failed to open cart file: ") + filename);
	}
	pico_api::load(data);
}

int safe_main(int argc, char** argv) {
	//	GFX_Init(config::INIT_SCREEN_WIDTH * 4, config::INIT_SCREEN_HEIGHT * 4);
	GFX_Init(512 * 3, 256 * 3);
	GFX_CreateBackBuffer(config::INIT_SCREEN_WIDTH, config::INIT_SCREEN_HEIGHT);
	pico_control::init();
	pico_data::load_font_data();

	if (argc == 1) {
		load_cart(FILE_GetDefaultCartName());
	} else {
		if (argc > 1) {
			load_cart(argv[1]);
		} else {
			return 1;
		}
	}

	uint32_t target_ticks = 20;
	uint32_t ticks = 0;
	uint32_t target_fps = 30;
	uint32_t actual_fps = 30;
	uint32_t sys_fps = 60;
	uint32_t cpu_usage = 0;

	uint32_t systemFrameCount = 0;
	uint32_t gameFrameCount = 0;
	uint32_t frameTimer = TIME_GetTime_ms();

	uint64_t updateTime = 0;
	uint64_t drawTime = 0;
	uint64_t copyBBTime = 0;

	bool init = false;
	bool restarted = true;
	bool script_error = false;

	while (EVT_ProcessEvents()) {
		using namespace pico_api;

		if (restarted == true) {
			restarted = false;
			script_error = false;
			init = false;
		}

		target_fps = pico_script::symbolExist("_update60") ? 60 : 30;
		HAL_SetFrameRates(target_fps, actual_fps, sys_fps, cpu_usage);
		pico_api::update_fps(target_fps, actual_fps, sys_fps, cpu_usage);

		int x, y;
		GFX_GetDisplayArea(&x, &y);
		pico_api::update_display_area(x, y);

		pico_api::set_time(TIME_GetTime_ms());

		if ((TIME_GetTime_ms() - ticks) > target_ticks) {
			HAL_StartFrame();
			pico_control::frame_start();

			if (!script_error) {
				try {
					if (!init) {
						pico_script::run("_init", true, restarted);
						init = true;
					}

					pico_script::run("_pre_update", true, restarted);
					pico_control::set_input_state(INP_GetInputState());

					if (pico_control::is_pause_menu()) {
						if (pico_script::do_menu()) {
							pico_control::end_pause_menu();
						}
					} else {
						uint64_t updateTimeStart = TIME_GetProfileTime();
						if (!pico_script::run("_update", true, restarted)) {
							if (pico_script::run("_update60", true, restarted)) {
								target_ticks = 1;
							}
						}
						updateTime += TIME_GetElapsedProfileTime_us(updateTimeStart);

						uint64_t drawTimeStart = TIME_GetProfileTime();
						pico_script::run("_draw", true, restarted);
						drawTime += TIME_GetElapsedProfileTime_us(drawTimeStart);
					}
				} catch (pico_script::error& e) {
					pico_control::displayerror(e.what());
					script_error = true;
				}
			}

			// call flip() even though this does not do anything, some carts implement their
			// own version to make end of frame.
			pico_script::run("flip", true, restarted);

			int buffer_w;
			int buffer_h;
			pico_api::colour_t* buffer = pico_control::get_buffer(buffer_w, buffer_h);
			uint64_t copyBBStart = TIME_GetProfileTime();
			GFX_SetBackBufferSize(buffer_w, buffer_h);
			GFX_CopyBackBuffer(buffer, buffer_w, buffer_h, pico_api::get_screen_palette());
			copyBBTime += TIME_GetElapsedProfileTime_us(copyBBStart);

			ticks = TIME_GetTime_ms();
			gameFrameCount++;

			pico_control::frame_end();
			HAL_EndFrame();
		}
		systemFrameCount++;
		GFX_Flip();

		if (TIME_GetElapsedTime_ms(frameTimer) >= 1000) {
			updateTime /= gameFrameCount;
			drawTime /= gameFrameCount;
			copyBBTime /= gameFrameCount;
			actual_fps = gameFrameCount;
			sys_fps = systemFrameCount;
			cpu_usage = ((updateTime + drawTime) * 100) / (target_fps == 60 ? 16666 : 33333);
			gameFrameCount = 0;
			systemFrameCount = 0;
			updateTime = 0;
			drawTime = 0;
			copyBBTime = 0;
			frameTimer = TIME_GetTime_ms();
		}
	}

	return 0;
}

int main(int argc, char** argv) {
	try {
		safe_main(argc, argv);
	} catch (gfx_exception& err) {
	} catch (pico_script::error& err) {
	} catch (pico_cart::error& err) {
	} catch (std::exception& err) {
	}

	pico_script::unload_scripting();
	GFX_End();

	return 0;
}
