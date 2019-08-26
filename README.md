# hmm

`hmm` is a <b>h</b>eight<b>m</b>ap <b>m</b>eshing utility.

If you've done any 3D game development, 3D printing, or other such things, you've likely wanted to convert
a grayscale heighmap image into a 3D mesh. The naive way is pretty simple but generates huge meshes with
millions of triangles. After hacking my way through such solutions over the years, I finally decided I needed to write
a good tool for this purpose.

`hmm` is a modern implementation of a nice algorithm from the 1995 paper [Fast Polygonal Approximation of
Terrains and Height Fields](http://mgarland.org/files/papers/scape.pdf) by Garland and Heckbert. The meshes produced by `hmm` satisfy the Delaunay condition and can satisfy a specified maximal error or maximal number of
triangles or vertices. It's also very fast.

### Installation

```bash
brew install boost glm # on macOS
git clone https://github.com/fogleman/hmm.git
cd hmm
make
```

### Usage

`hmm` supports a variety of file formats like PNG, JPG, etc. for the input heightmap. The output is always a binary STL file.
The only other required parameter is ZSCALE, which specifies how much to scale the Z axis in the output mesh.

    ./hmm input.png output.stl -z ZSCALE

You can also provide a maximal allowed error, number of triangles, or number of vertices.
(If multiple are specified, the first one reached is used.)

    ./hmm input.png output.stl -z 100 -e 0.001 -t 1000000

### TODO

- add a solid base (coming soon)
- pre-triangulation filters? e.g. gaussian blur
- export a normal map?
- automatically compute some z scale?
- better error handling, especially for file I/O
- better overflow handling - what's the largest supported heightmap?
- thread pool doesn't help much usually - get rid of it?
- OpenCL rasterization?
- mesh validation?
