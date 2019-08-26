# hmm

`hmm` is a <b>h</b>eight<b>m</b>ap <b>m</b>eshing utility.

If you've done any 3D game development, 3D printing, or other such things,
you've likely wanted to convert a grayscale heighmap image into a 3D mesh. The
naive way is pretty simple but generates huge meshes with millions of
triangles. After hacking my way through various solutions over the years, I
finally decided I needed to write a good tool for this purpose.

`hmm` is a modern implementation of a nice algorithm from the 1995 paper
[Fast Polygonal Approximation of Terrains and Height Fields](http://mgarland.org/files/papers/scape.pdf)
by Garland and Heckbert. The meshes produced by `hmm` satisfy the Delaunay
condition and can satisfy a specified maximal error or maximal number of
triangles or vertices. It's also very fast.

![Example](https://i.imgur.com/2yNhUSV.png)

### Installation

```bash
brew install glm # on macOS
git clone https://github.com/fogleman/hmm.git
cd hmm
make
```

### Usage

```
usage: ./hmm --zscale=float [options] ... infile outfile.stl
options:
  -z, --zscale       z scale relative to x & y (float)
  -x, --zexagg       z exaggeration (float [=1])
  -e, --error        maximum triangulation error (float [=0.001])
  -t, --triangles    maximum number of triangles (int [=0])
  -p, --points       maximum number of vertices (int [=0])
  -b, --base         solid base height (float [=0])
  -q, --quiet        suppress console output
  -?, --help         print this message
```

`hmm` supports a variety of file formats like PNG, JPG, etc. for the input
heightmap. The output is always a binary STL file. The only other required
parameter is ZSCALE, which specifies how much to scale the Z axis in the output
mesh.

    ./hmm input.png output.stl -z ZSCALE

You can also provide a maximal allowed error, number of triangles, or number of
vertices. (If multiple are specified, the first one reached is used.)

    ./hmm input.png output.stl -z 100 -e 0.001 -t 1000000

### TODO

- pre-triangulation filters? e.g. gaussian blur
- export a normal map?
- automatically compute some z scale?
- better error handling, especially for file I/O
- better overflow handling - what's the largest supported heightmap?
- thread pool doesn't help much usually - get rid of it?
- OpenCL rasterization?
- mesh validation?
