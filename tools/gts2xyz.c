#include <gts.h>
#include <stdlib.h>

typedef struct {
  GtsVector min, max;
} Range;

static void range (GtsPoint * p, Range * r)
{
  int c;
  for (c = 0; c < 3; c++) {
    if ((&p->x)[c] > r->max[c])
      r->max[c] = (&p->x)[c];
    if ((&p->x)[c] < r->min[c])
      r->min[c] = (&p->x)[c];
  }
}

int main (int argc, char * argv[])
{
  if (argc != 2) {
    fprintf (stderr, 
	     "Usage: gts2xyz DX < input.gts > output.xyz\n"
	     "Samples a GTS file on a regular grid at intervals DX\n");
    return 1;
  }

  GtsSurface * s = gts_surface_new (gts_surface_class (),
				    gts_face_class (),
				    gts_edge_class (),
				    gts_vertex_class ());
  GtsFile * fp = gts_file_new (stdin);
  if (gts_surface_read (s, fp)) {
    fputs ("gts2xyz: the file on standard input is not a valid GTS file\n", stderr);
    fprintf (stderr, "stdin:%d:%d: %s\n", fp->line, fp->pos, fp->error);
    return 1; /* failure */
  }

  Range r = {{G_MAXDOUBLE,G_MAXDOUBLE,G_MAXDOUBLE}, 
	     {-G_MAXDOUBLE,-G_MAXDOUBLE,-G_MAXDOUBLE}};
  gts_surface_foreach_vertex (s, (GtsFunc) range, &r);
  double dx = atof (argv[1]);
  GtsFace * f = NULL;
  GtsPoint p;
  for (p.x = r.min[0]; p.x <= r.max[0]; p.x += dx)
    for (p.y = r.min[1]; p.y <= r.max[1]; p.y += dx) {
      f = gts_point_locate (&p, s, f);
      if (f != NULL) {
	gts_triangle_interpolate_height (GTS_TRIANGLE (f), &p);
	if (p.z > r.min[2] && p.z < r.max[2])
	  printf ("%f %f %f\n", p.x, p.y, p.z);
      }
    }

  return 0;
}
