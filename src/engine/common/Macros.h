#pragma once

#ifdef ENGINE__TEST
#define DEPINJ(label, file) DEPINJ__##label
#else
#define DEPINJ(label, file) file
#endif