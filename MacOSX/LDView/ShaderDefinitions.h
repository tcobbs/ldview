//
//  ShaderDefinitions.h
//  MetalTest
//
//  Created by Travis Cobbs on 6/7/22.
//

#ifndef ShaderDefinitions_h
#define ShaderDefinitions_h

#ifdef __METAL_VERSION__
	// Metal Shading Language types
	#define simd_float4 float4
	#define simd_float3 float3
	#define simd_float2 float2
#else
	// CPU/Swift types
	#include <simd/simd.h>
#endif

struct Vertex {
    simd_float4 color;
    simd_float2 pos;
};

struct FragmentUniforms {
    float brightness;
    float currentTime;
};

#endif /* ShaderDefinitions_h */
