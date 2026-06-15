//
//  LDrawMetalView_mm.m
//  LDView
//
//  Created by Travis Cobbs on 6/14/26.
//

#import "LDrawMetalView.h"
#include "ShaderDefinitions.h"
#include <math.h>

NS_ASSUME_NONNULL_BEGIN

static simd::float4x4 z_rotation(float theta)
{
	float c = cosf(theta);
	float s = sinf(theta);
	simd::float4 col0 = {   c,    s, 0.0f, 0.0f};
	simd::float4 col1 = {  -s,    c, 0.0f, 0.0f};
	simd::float4 col2 = {0.0f, 0.0f, 1.0f, 0.0f};
	simd::float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};
	return simd_matrix(col0, col1, col2, col3);
}

static simd::float4x4 scale(float amt)
{
	simd::float4 col0 = { amt, 0.0f, 0.0f, 0.0f};
	simd::float4 col1 = {0.0f,  amt, 0.0f, 0.0f};
	simd::float4 col2 = {0.0f, 0.0f,  amt, 0.0f};
	simd::float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};
	return simd_matrix(col0, col1, col2, col3);
}

static simd::float4x4 translate(simd::float4 ofs)
{
	simd::float4 col0 = {1.0f, 0.0f, 0.0f, 0.0f};
	simd::float4 col1 = {0.0f, 1.0f, 0.0f, 0.0f};
	simd::float4 col2 = {0.0f, 0.0f, 1.0f, 0.0f};
	simd::float4 col3 = {ofs[0], ofs[1], ofs[2], 1.0f};
	return simd_matrix(col0, col1, col2, col3);
}

@implementation LDrawMetalView

@synthesize commandQueue;
@synthesize pipelineState;
@synthesize vertexBuffer;
@synthesize indexBuffer;
@synthesize fragmentUniformsBuffer;

+ (id<MTLRenderPipelineState>)buildRenderPipelineWithDevice:(id<MTLDevice>)device mtkView:(MTKView*)view {
	MTLRenderPipelineDescriptor *pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
	id<MTLLibrary> library = [device newDefaultLibrary];
	pipelineDescriptor.vertexFunction = [library newFunctionWithName:@"vertexShader"];
	pipelineDescriptor.fragmentFunction = [library newFunctionWithName: @"fragmentShader"];
	pipelineDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
	return [device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:NULL];
}

- (void)setupBuffers
{
	struct Vertex vertices[] = {
		{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f } },
		{ { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
		{ { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
		{ { 0.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, -1.0f } },
	};
	self.vertexBuffer = [self.device newBufferWithBytes:vertices length:sizeof vertices options:0];
	uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };
	numIndices = (NSUInteger)(sizeof indices / sizeof indices[0]);
	self.indexBuffer = [self.device newBufferWithBytes:indices length:sizeof indices options:0];
	struct FragmentUniforms uniforms = { 1.0f, 0.0f };
	self.fragmentUniformsBuffer = [self.device newBufferWithBytes:&uniforms length:sizeof uniforms options:0];
}

- (void)awakeFromNib
{
	gpuLock = dispatch_semaphore_create(1);
	self.device = MTLCreateSystemDefaultDevice();
	NSLog(@"device: %@", self.device);
	self.commandQueue = [self.device newCommandQueue];
	self.pipelineState = [LDrawMetalView buildRenderPipelineWithDevice:self.device mtkView:self];
	[self setupBuffers];
}

- (float)update:(CFTimeInterval)dt
{
	struct FragmentUniforms *ptr = (FragmentUniforms*)[fragmentUniformsBuffer contents];
	ptr->brightness = (float)(0.5 * cos(currentTime) + 0.5);
	ptr->currentTime = (float)currentTime;
	currentTime += dt;
	return ptr->brightness;
//	[fragmentUniformsBuffer didModifyRange:NSMakeRange(0, sizeof(struct FragmentUniforms))];
}

- (void)drawSquareAtX:(float)x y:(float)y renderEncoder:(id<MTLRenderCommandEncoder>)renderEncoder brightness:(float)brightness
{
	[renderEncoder setVertexBuffer:self.vertexBuffer offset:0 atIndex:0];
	simd::float4x4 matrix =
		translate(simd::float4{x, y, 0.0f, 0.0f}) *
		z_rotation((float)(currentTime * 2.0f)) *
		scale(brightness * 0.4f + 0.1f);
	[renderEncoder setVertexBytes:&matrix length:sizeof(matrix) atIndex:1];
	[renderEncoder setFragmentBuffer:self.fragmentUniformsBuffer offset:0 atIndex:0];
	[renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:numIndices indexType:MTLIndexTypeUInt16 indexBuffer:self.indexBuffer indexBufferOffset:0];
}

- (void)drawRect:(NSRect) dirtyRect
{
	dispatch_semaphore_wait(gpuLock, DISPATCH_TIME_FOREVER);
	CFTimeInterval systemTime = CACurrentMediaTime();
	CFTimeInterval timeDifference = (lastRenderTime == 0.0f) ? 0 : (systemTime - lastRenderTime);
	lastRenderTime = systemTime;
	float brightness = [self update:timeDifference];
	id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
	MTLRenderPassDescriptor *renderPassDescriptor = self.currentRenderPassDescriptor;
	renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
	id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
	[renderEncoder setRenderPipelineState:pipelineState];
	[self drawSquareAtX:-0.5f y:-0.5f renderEncoder:renderEncoder brightness:brightness];
	[self drawSquareAtX:-0.5f y:0.5f renderEncoder:renderEncoder brightness:brightness];
	[self drawSquareAtX:0.5f y:0.5f renderEncoder:renderEncoder brightness:brightness];
	[self drawSquareAtX:0.5f y:-0.5f renderEncoder:renderEncoder brightness:brightness];
	[renderEncoder endEncoding];
	[commandBuffer presentDrawable:self.currentDrawable];
	[commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> _) {
		dispatch_semaphore_signal(gpuLock);
	}];
	[commandBuffer commit];
}

@end

NS_ASSUME_NONNULL_END
