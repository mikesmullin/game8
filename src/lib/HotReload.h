#pragma once

#include "../game/Logic.h"

extern logic_oninit_data_t logic_oninit_data;
extern logic_oninit_compute_t logic_oninit_compute;
extern logic_onreload_t logic_onreload;
extern logic_onfixedupdate_t logic_onfixedupdate;
extern logic_onupdate_t logic_onupdate;

int load_logic(const char* file);
int unload_logic(void);
