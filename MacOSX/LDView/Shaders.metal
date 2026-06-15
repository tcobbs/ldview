//
//  Shaders.metal
//  LDView
//
//  Created by Travis Cobbs on 6/14/26.
//

#include <metal_stdlib>
#include "ShaderDefinitions.h"

using namespace metal;

struct VertexOut {
	float4 color;
	float4 pos [[position]];
};

vertex VertexOut vertexShader(
	const device Vertex *vertexArray [[buffer(0)]],
	unsigned int vid [[vertex_id]],
	constant float4x4& transform [[buffer(1)]])
{
	Vertex in = vertexArray[vid];
	VertexOut out;
	out.color = in.color;
	out.pos = float4(transform * float4(in.pos.x, in.pos.y, 0, 1));
	return out;
}

fragment float4 fragmentShader(VertexOut interpolated [[stage_in]], constant FragmentUniforms &uniforms [[buffer(0)]])
{
	return float4(uniforms.brightness/*(0.5 * cos(uniforms.currentTime) + 0.5)*/ * interpolated.color.rgb, interpolated.color.a);
//	return float4(interpolated.color.rgb, uniforms.brightness);
}
