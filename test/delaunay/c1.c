/* GTS - Library for the manipulation of triangulated surfaces
 * Copyright (C) 1999 Stéphane Popinet
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "gts.h"

int main (int argc, char * argv[])
{
  GtsSurface * surface = gts_surface_new (gts_surface_class (),
                                          gts_face_class (),
                                          gts_edge_class (),
                                          gts_vertex_class ());

  GtsVertex * v1 = gts_vertex_new (gts_vertex_class (), 0, 0, 0);
  GtsVertex * v2 = gts_vertex_new (gts_vertex_class (), 0.5, 0.5, 0);
  GtsVertex * v3 = gts_vertex_new (gts_vertex_class (), 0, 0.5, 0);
  GtsVertex * v4 = gts_vertex_new (gts_vertex_class (), 0, 1, 0);
  GtsVertex * v5 = gts_vertex_new (gts_vertex_class (), -0.5, 0.5, 0);

  GtsEdge * e1 = gts_edge_new (GTS_EDGE_CLASS (gts_constraint_class ()), 
			       v1, v2);
  GtsEdge * e2 = gts_edge_new (GTS_EDGE_CLASS (gts_constraint_class ()), 
			       v3, v2);
  GtsEdge * e3 = gts_edge_new (GTS_EDGE_CLASS (gts_constraint_class ()), 
			       v1, v3);
  GtsEdge * e4 = gts_edge_new (gts_edge_class (), v1, v4);
  GtsEdge * e5 = gts_edge_new (GTS_EDGE_CLASS (gts_constraint_class ()), 
			       v4, v5);
  GtsEdge * e6 = gts_edge_new (GTS_EDGE_CLASS (gts_constraint_class ()), 
			       v1, v5);
  GtsEdge * e7 = gts_edge_new (GTS_EDGE_CLASS (gts_constraint_class ()), 
			       v4, v2);
  GtsEdge * e8 = gts_edge_new (GTS_EDGE_CLASS (gts_constraint_class ()), 
			       v3, v4);
  GtsEdge * e9 = gts_edge_new (GTS_EDGE_CLASS (gts_constraint_class ()), 
			       v5, v3);

  GtsFace * f1 = gts_face_new (gts_face_class (),
			       e1, e2, e3);
  GtsFace * f2 = gts_face_new (gts_face_class (),
			       e4, e5, e6);
  GtsFace * f3 = gts_face_new (gts_face_class (),
			       e7, e8, e2);
  GtsFace * f4 = gts_face_new (gts_face_class (),
			       e4, e3, e8);
  
  gts_surface_add_face (surface, f1);
  gts_surface_add_face (surface, f2);
  gts_surface_add_face (surface, f3);
  gts_surface_add_face (surface, f4);

  gts_delaunay_add_constraint (surface, GTS_CONSTRAINT (e9));

#if 0
  gts_surface_print_stats (surface, stderr);
  gts_surface_write (surface, stdout);
#endif

  return 0;
}
