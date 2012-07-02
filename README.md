GNU Triangulated Surface Library
================================

**GTS** stands for the GNU Triangulated Surface Library. It is an Open Source Free Software Library intended to provide a set of useful functions to deal with 3D surfaces meshed with interconnected triangles. The source code is distributed under the terms of the  [GNU Lesser General Public License (LGPL)](http://www.gnu.org/copyleft/lgpl.html).

The code is written entirely in C with an object-oriented approach based mostly on the design of  [GTK+](http://www.gtk.org/). Careful attention is paid to performance related issues as the initial goal of **gts** is to provide a simple and efficient library to scientists dealing with 3D computational surface meshes.

A brief summary of its main features:

 * Simple object-oriented structure giving easy access to topological properties. 
 * 2D dynamic Delaunay and constrained Delaunay triangulations. 
 * Robust geometric predicates (orientation, in circle) using fast adaptive floating point arithmetic (adapted from [the fine work of Jonathan R. Shewchuk](http://www.cs.cmu.edu/~quake/robust.html)).
 * Robust set operations on surfaces (union, intersection, difference).
 * Surface refinement and coarsening (multiresolution models).
 * Dynamic view-independent continuous level-of-detail.
 * Preliminary support for view-dependent level-of-detail. 
 * Bounding-boxes trees and Kd-trees for efficient point location and collision/intersection detection.
 * Graph operations: traversal, graph partitioning.
 * Metric operations (area, volume, curvature ...).
 * Triangle strips generation for fast rendering.
 * Simple try/catch/throw exception-handling interface using [cexcept](http://www.nicemice.net/cexcept/).
