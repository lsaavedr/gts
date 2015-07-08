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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.         See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <locale.h>

#include "gts.h"

volatile FILE * fptr;

static void write_vertex (GtsVertex * v, guint * nv)
{
  fprintf (fptr, "v %g %g %g\n", GTS_POINT (v)->x, GTS_POINT (v)->y, GTS_POINT (v)->z);
  GTS_OBJECT (v)->reserved = GUINT_TO_POINTER ((*nv)++);
}

static void write_face (GtsTriangle * t)
{
  GtsVertex * v1, * v2, * v3;

  gts_triangle_vertices (t, &v1, &v2, &v3);
  fprintf (fptr, "f %u %u %u\n",
          GPOINTER_TO_UINT (GTS_OBJECT (v1)->reserved),
          GPOINTER_TO_UINT (GTS_OBJECT (v2)->reserved),
          GPOINTER_TO_UINT (GTS_OBJECT (v3)->reserved));
}

int main (int argc, char * argv[])
{
  GtsSurface * s;
  GtsFile * fp;
  guint nv = 1;

  if (!setlocale (LC_ALL, "POSIX"))
    g_warning ("cannot set locale to POSIX");

  s = gts_surface_new (gts_surface_class (),
                       gts_face_class (),
                       gts_edge_class (),
                       gts_vertex_class ());

  /* open first file */
  FILE * fptr_in;
  if ((fptr_in = fopen (argv[1], "rt")) == NULL) {
    fprintf (stderr, "set: can not open file `%s'\n", argv[1]);
    return 1;
  }
  if ((fptr = fopen (argv[2], "wt")) == NULL) {
    fprintf (stderr, "set: can not open file `%s'\n", argv[2]);
    return 1;
  }

  fp = gts_file_new (fptr_in);
  if (gts_surface_read (s, fp)) {
    fputs ("gts2obj: file on standard input is not a valid GTS file\n",
           stderr);
    fprintf (stderr, "stdin:%d:%d: %s\n", fp->line, fp->pos, fp->error);
    return 1; /* failure */
  }

  gts_surface_foreach_vertex (s, (GtsFunc) write_vertex, &nv);
  gts_surface_foreach_face (s, (GtsFunc) write_face, NULL);

  fclose(fptr);
  return 0;
}
