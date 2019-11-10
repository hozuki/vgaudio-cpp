#pragma once

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define _CONCAT_0(x, y) x##y
#define CONCAT(x, y) _CONCAT_0(x, y)

#define DONT_DELETE(type_name) [](type_name*){}
