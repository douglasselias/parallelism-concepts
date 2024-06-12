# parallelism-concepts

This is a summary of the most common parallelism concepts that I know of and serves as a reference guide for me.

## 


## Threads VS Goroutines

When you start learning Go and Goroutines, you eventually wonder what is the difference between a thread and a Goroutine.
Probably most content on the internet will say that is expensive to create and manage threads, which is true, and show
how Goroutine has a better solution, since it is lightweight.

One website defines the benefit of a Goroutine like this:

> It's extremely cheap to create and use goroutines compared to creating and working with threads. Goroutines are a few kilobytes in size and the stack can grow and shrink according to the needs of the application. On the other hand, thread stack size is fixed and it cannot grow and shrink.

This mentality is similar to manual memory management, where you are taught to use only what you need, otherwise you would be wasting resources. The truth is that this idea makes sense in a small environment, but in the real world you want to maximize the resource usage.

To exemplify this, I created program `pgm.c` that creates a gradient image file with 1000 by 1000 pixels. PGM is a really simple image file format.
Then, I created a `reverse_pgm.c` and `reverse_pgm.go` that reads this image and invert the pixel color.

My approach for the Go code was to create a Goroutine for each pixel of the image. For the C code, my approach was to divide
the image in chunk based on the number of CPU cores available.

I used the `time` command on linux to compare the time and CPU usage.

Go results:
![go results](threads_vs_goroutines/go_results.png)

C results:
![c results](threads_vs_goroutines/c_results.png)

The C code executes instantly and the Go code takes at least half a second. I couldn't find a way to measure the memory used by each program, but I can infer that Go uses more memory since each Goroutine needs its own stack.

Conclusion: For now I cannot think of any situation where it would be desirable to have Goroutines.