//
//  LDrawMetalView_mm.h
//  LDView
//
//  Created by Travis Cobbs on 6/14/26.
//

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface LDrawMetalView : MTKView
{
	dispatch_semaphore_t gpuLock;
	CFTimeInterval lastRenderTime;
	CFTimeInterval currentTime;
	NSUInteger numIndices;
}

@property (nonatomic, strong) id<MTLCommandQueue> commandQueue;
@property (nonatomic, strong) id<MTLRenderPipelineState> pipelineState;
@property (nonatomic, strong) id<MTLBuffer> vertexBuffer;
@property (nonatomic, strong) id<MTLBuffer> indexBuffer;
@property (nonatomic, strong) id<MTLBuffer> fragmentUniformsBuffer;

@end

NS_ASSUME_NONNULL_END
