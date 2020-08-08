A simple nBody Simulator
==================

## for practicing OpenGL compute shaders

![](imgs/galaxy.gif)

## Collision of two galaxies each consisting of 26k stars

![](imgs/galaxycollision.gif)

## explanations:

- Performance was greatly enhanced by utilizing a tiled based parallelization method introduced in [Fast N-Body Simulation with CUDA in GPU Gem 3](https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-31-fast-n-body-simulation-cuda)
 ![imgs/gem1.jpg]()
- As is shown in the diagram above, the inter-body force cauculation happens in parallel as each thread in a tile (in the same workgroup as is for the case of openGL compute shaders) compute force between the thread's "nbody" and target "nbody" for the entire tile or work group, 
a shared vec4 buffer is also used serving as a cache for threads inside the same workgroup to share positional data and integrate force calculations.
- After or at the same time the calculation in one workgroup finishes, another workgroup would be scheduled for same kind of calculation and add back to the overall forces, in the end, we would have a total force calculation which could have taken O(N^2) to finish much faster.
- The reason why we choose tile based method is due to the memory bandwith limitation of GPUs according to Gem, as for OpenGL, the limitation is on the size of workgroup, which I choosed to be 256.
- Bloom was also implemented by using gaussian blur of brightness ping-pong textures attached to a framebuffer, referecned [online Gaussian Kernel Calculator](http://dev.theomader.com/gaussian-kernel-calculator/)
