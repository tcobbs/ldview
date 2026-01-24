#define STB_IMAGE_RESIZE_IMPLEMENTATION
// All used single-file STB headers should be included below.

#ifdef COCOA
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-conversion"
#endif // COCOA
#include "../3rdParty/stb/stb_image_resize2.h"
#ifdef COCOA
#pragma clang diagnostic pop
#endif // COCOA
