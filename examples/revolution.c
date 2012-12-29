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

#include <stdlib.h>
#include <locale.h>
#include "config.h"
#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#endif /* HAVE_GETOPT_H */
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include "gts.h"

int main (int argc, char * argv[])
{
  GtsSurface * s;
  GSList * vertices = NULL, * edges = NULL, * i;
  GtsVertex * v = NULL;
  gdouble x, y;
  gboolean verbose = FALSE;
  int c = 0;

  if (!setlocale (LC_ALL, "POSIX"))
    g_warning ("cannot set locale to POSIX");

  s = gts_surface_new (gts_surface_class (),
		       gts_face_class (),
		       gts_edge_class (),
		       gts_vertex_class ());

  /* parse options using getopt */
  while (c != EOF) {
#ifdef HAVE_GETOPT_LONG
    static struct option long_options[] = {
      {"help", no_argument, NULL, 'h'},
      {"verbose", no_argument, NULL, 'v'},
      { NULL }
    };
    int option_index = 0;
    switch ((c = getopt_long (argc, argv, "hv", 
			      long_options, &option_index))) {
#else /* not HAVE_GETOPT_LONG */
    switch ((c = getopt (argc, argv, "hv"))) {
#endif /* not HAVE_GETOPT_LONG */
    case 'v': /* verbose */
      verbose = TRUE;
      break;
    case 'h': /* help */
      fprintf (stderr,
             "Usage: revolution [OPTION] < FILE\n"
	     "Reads profile in FILE and generates a GTS surface of revolution.\n"
	     "\n"
	     "  -v      --verbose  print statistics about the surface\n"
	     "  -h      --help     display this help and exit\n"
	     "\n"
	     "Report bugs to %s\n",
	     GTS_MAINTAINER);
      return 0; /* success */
      break;
    case '?': /* wrong options */
      fprintf (stderr, "Try `cleanup --help' for more information.\n");
      return 1; /* failure */
    }
  }

  while (scanf ("%lf %lf", &x, &y) == 2)
    vertices = g_slist_prepend (vertices, gts_vertex_new (gts_vertex_class (), x, y, 0.));
  i = vertices;
  while (i) {
    if (v)
      edges = g_slist_prepend (edges, gts_edge_new (gts_edge_class (), v, i->data));
    v = i->data;
    i = i->next;
  }
  g_slist_free (vertices);

  
  



  m = gts_surface_new (gts_surface_class (),
		       gts_face_class (),
		       gts_edge_class (),
		       gts_vertex_class ());
  fp = gts_file_new (stdin);
  if (gts_surface_read (m, fp)) {
    fputs ("cleanup: file on standard input is not a valid GTS file\n", 
	   stderr);
    fprintf (stderr, "stdin:%d:%d: %s\n", fp->line, fp->pos, fp->error);
    return 1; /* failure */
  }
  gts_surface_merge (s, m);
  gts_object_destroy (GTS_OBJECT (m));

  /* if verbose on print stats */
  if (verbose) 
    gts_surface_print_stats (s, stderr);
 
  /* merge vertices which are close enough */
  /* build list of vertices */
  gts_surface_foreach_vertex (s, boundary ? (GtsFunc) build_list2 : (GtsFunc) build_list1, 
			      &vertices);
  /* merge vertices: we MUST update the variable vertices because this function
     modifies the list (i.e. removes the merged vertices). */
  vertices = gts_vertices_merge (vertices, threshold);

  /* free the list */
  g_list_free (vertices);

  /* eliminate degenerate and duplicate edges */
  edge_cleanup (s);
  /* eliminate duplicate triangles */
  triangle_cleanup (s);

  if (sever)
    gts_surface_foreach_vertex (s, (GtsFunc) vertex_cleanup, NULL);

  /* if verbose on print stats */
  if (verbose) {
    GtsBBox * bb = gts_bbox_surface (gts_bbox_class (), s);
    gts_surface_print_stats (s, stderr);
    fprintf (stderr, "# Bounding box: [%g,%g,%g] [%g,%g,%g]\n",
	     bb->x1, bb->y1, bb->z1,
	     bb->x2, bb->y2, bb->z2);	     
  }

  /* write surface */
  gts_surface_write (s, stdout);

  return 0; /* success */
}
