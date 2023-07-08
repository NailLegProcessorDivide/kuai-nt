#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <optional>

#include <string>
#include <sstream>
#include <vector>

#include <filesystem>

#include "kuai/Core/Log.h"
#include "kuai/Util/Instrumentor.h"

#ifdef KU_PLATFORM_WINDOWS
	#include <Windows.h>
#endif