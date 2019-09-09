#ifndef _MIXX_CONFIG_H_
#define _MIXX_CONFIG_H_

#if defined(_DEBUG)
#define MIXX_DEBUG_ENABLE_LEAK_DETECTION
#endif // !_DEBUG

namespace mixx {

constexpr unsigned DefaultContainerCapacity = 256;

}

#endif // !_MIXX_CONFIG_H_
