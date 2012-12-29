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

typedef struct _GtsCEdge         GtsCEdge;
typedef struct _GtsCEdgeClass    GtsCEdgeClass;
typedef struct _GtsCurve         GtsCurve;
typedef struct _GtsCurveClass    GtsCurveClass;
typedef enum 
{ 
  GTS_DIRECT = -1,
  GTS_INVERSE = 1
} GtsOrientation;

struct _GtsCEdge {
  GtsEdge constraint;
  
  GSList * curves;
};

struct _GtsCEdgeClass {
  GtsEdgeClass parent_class;
};

#define GTS_CEDGE(obj)            GTS_OBJECT_CAST (obj,\
						   GtsCEdge,\
						   gts_cedge_class ())
#define GTS_CEDGE_CLASS(klass)    GTS_OBJECT_CLASS_CAST (klass,\
							 GtsCEdgeClass,\
							 gts_cedge_class())
#define GTS_IS_CEDGE(obj)         (gts_object_is_from_class (obj,\
						   gts_cedge_class ()))
     
GtsCEdgeClass * gts_cedge_class             (void);
GtsCEdge *      gts_cedge_new               (GtsCEdgeClass * klass,
					     GtsVertex * v1,
					     GtsVertex * v2);
gboolean        gts_cedge_has_parent_curve  (GtsCEdge * ce,
					     GtsCurve * curve);
void            gts_cedge_replace           (GtsCEdge * ce, 
					     GtsCEdge * with);

struct _GtsCurve {
  GtsObject object;

  GHashTable * edges;
  GtsCEdgeClass * cedge_class;
  GtsVertexClass * vertex_class;
  gboolean keep_edges;
};

struct _GtsCurveClass {
  GtsObjectClass parent_class;
};

#define GTS_CURVE(obj)            GTS_OBJECT_CAST (obj,\
					           GtsCurve,\
					           gts_curve_class ())
#define GTS_CURVE_CLASS(klass)    GTS_OBJECT_CLASS_CAST (klass,\
						         GtsCurveClass,\
						         gts_curve_class())
#define GTS_IS_CURVE(obj)         (gts_object_is_from_class (obj,\
						   gts_curve_class ()))
     
GtsCurveClass * gts_curve_class           (void);
GtsCurve *      gts_curve_new             (GtsCurveClass * klass,
					   GtsCEdgeClass * cedge_class,
					   GtsVertexClass * vertex_class);
void            gts_curve_add_edge        (GtsCurve * curve, 
					   GtsCEdge * ce,
					   GtsOrientation orient);
void            gts_curve_remove_edge     (GtsCurve * curve, 
					   GtsCEdge * ce);
gboolean        gts_allow_floating_cedges = FALSE;
GtsOrientation  gts_curve_edge_orientation (GtsCurve * curve, 
					    GtsCEdge * ce);
void            gts_curve_foreach_edge    (GtsCurve * curve,
					   GtsFunc func, 
					   gpointer data);
void            gts_curve_foreach_vertex  (GtsCurve * curve,
					   GtsFunc func,
					   gpointer data);

static void cedge_destroy (GtsObject * object)
{
  GtsCEdge * ce = GTS_CEDGE (object);
  GSList * i;

  i = ce->curves;
  while (i) {
    GSList * next = i->next;
    gts_curve_remove_edge (i->data, ce);
    i = next;
  }
  g_assert (ce->curves == NULL);

  (* GTS_OBJECT_CLASS (gts_cedge_class ())->parent_class->destroy) (object);
}

static void cedge_class_init (GtsCEdgeClass * klass)
{
  GTS_OBJECT_CLASS (klass)->destroy = cedge_destroy;
}

static void cedge_init (GtsCEdge * object)
{
  object->curves = NULL;
}

GtsCEdgeClass * gts_cedge_class (void)
{
  static GtsCEdgeClass * klass = NULL;

  if (klass == NULL) {
    GtsObjectClassInfo cedge_info = {
      "GtsCEdge",
      sizeof (GtsCEdge),
      sizeof (GtsCEdgeClass),
      (GtsObjectClassInitFunc) cedge_class_init,
      (GtsObjectInitFunc) cedge_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
    };
    klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_constraint_class ()),
				  &cedge_info);
  }

  return klass;
}

GtsCEdge * gts_cedge_new (GtsCEdgeClass * klass,
			  GtsVertex * v1,
			  GtsVertex * v2)
{
  GtsCEdge * object;

  object = GTS_CEDGE (gts_edge_new (GTS_EDGE_CLASS (klass), v1, v2));

  return object;
}

gboolean gts_cedge_has_parent_curve (GtsCEdge * ce, GtsCurve * curve)
{
  GSList * i;

  g_return_val_if_fail (ce != NULL, FALSE);
  g_return_val_if_fail (curve != NULL, FALSE);

  i = ce->curves;
  while (i) {
    if (curve == i->data)
      return TRUE;
    i = i->next;
  }
  
  return FALSE;
}

void gts_cedge_replace (GtsCEdge * ce, GtsCEdge * with)
{
  GSList * i;
  GtsOrientation orient;

  g_return_if_fail (ce != NULL && with != NULL && ce != with);

  gts_edge_replace (GTS_EDGE (ce), GTS_EDGE (with));

  orient = GTS_SEGMENT (ce)->v1 == GTS_SEGMENT (with)->v1 
    ? GTS_DIRECT : GTS_INVERSE;

  i = ce->curves;
  while (i) {
    gts_curve_add_edge (i->data, with,
			orient*gts_curve_edge_orientation (i->data, ce));
    i = i->next;
  }
}

static void destroy_foreach_edge (GtsCEdge * ce, GtsCurve * curve)
{
  ce->curves = g_slist_remove (ce->curves, curve);
  if (!GTS_OBJECT_DESTROYED (ce) &&
      !gts_allow_floating_cedges && ce->curves == NULL)
    gts_object_destroy (GTS_OBJECT (ce));
}

static void curve_destroy (GtsObject * object)
{
  GtsCurve * curve = GTS_CURVE (object);
  
  gts_curve_foreach_edge (curve, (GtsFunc) destroy_foreach_edge, 
			     curve);
  g_hash_table_destroy (curve->edges);

  (* GTS_OBJECT_CLASS (gts_curve_class ())->parent_class->destroy) (object);
}

static void curve_class_init (GtsCurveClass * klass)
{
  GTS_OBJECT_CLASS (klass)->destroy = curve_destroy;
}

static void curve_init (GtsCurve * curve)
{
  curve->edges = g_hash_table_new (NULL, NULL);
  curve->cedge_class = gts_cedge_class ();
  curve->vertex_class = gts_vertex_class ();
  curve->keep_edges = FALSE;
}

GtsCurveClass * gts_curve_class (void)
{
  static GtsCurveClass * klass = NULL;

  if (klass == NULL) {
    GtsObjectClassInfo curve_info = {
      "GtsCurve",
      sizeof (GtsCurve),
      sizeof (GtsCurveClass),
      (GtsObjectClassInitFunc) curve_class_init,
      (GtsObjectInitFunc) curve_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
    };
    klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_object_class ()),
				  &curve_info);
  }

  return klass;
}

GtsCurve * gts_curve_new (GtsCurveClass * klass,
			  GtsCEdgeClass * cedge_class,
			  GtsVertexClass * vertex_class)
{
  GtsCurve * curve;

  curve = GTS_CURVE (gts_object_new (GTS_OBJECT_CLASS (klass)));
  curve->cedge_class = cedge_class;
  curve->vertex_class = vertex_class;

  return curve;
}

void gts_curve_add_edge (GtsCurve * curve, 
			 GtsCEdge * ce,
			 GtsOrientation orient)
{
  g_return_if_fail (curve != NULL);
  g_return_if_fail (ce != NULL);
  
  g_assert (curve->keep_edges == FALSE);

  if (!g_hash_table_lookup (curve->edges, ce)) {
    ce->curves = g_slist_prepend (ce->curves, curve);
    g_hash_table_insert (curve->edges, ce, GINT_TO_POINTER (orient));
  }
}

void gts_curve_remove_edge (GtsCurve * curve, 
			    GtsCEdge * ce)
{
  g_return_if_fail (curve != NULL);
  g_return_if_fail (ce != NULL);

  g_assert (curve->keep_edges == FALSE);

  g_hash_table_remove (curve->edges, ce);
  ce->curves = g_slist_remove (ce->curves, curve);
  if (!GTS_OBJECT_DESTROYED (ce) &&
      !gts_allow_floating_cedges && 
      GTS_EDGE (ce)->triangles == NULL &&
      ce->curves == NULL)
    gts_object_destroy (GTS_OBJECT (ce));
}

GtsOrientation gts_curve_edge_orientation (GtsCurve * curve, GtsCEdge * ce)
{
  gpointer value;

  g_return_val_if_fail (curve != NULL, 0);
  g_return_val_if_fail (ce != NULL, 0);
  
  value = g_hash_table_lookup (curve->edges, ce);
  g_return_val_if_fail (value != NULL, 0);
  
  return GPOINTER_TO_INT (value);
}

static void foreach_edge (GtsCEdge * ce, 
			  gpointer ce_data,
			  gpointer * info)
{
  (*((GtsFunc) info[0])) (ce, info[1]);
}

void gts_curve_foreach_edge (GtsCurve * curve,
			     GtsFunc func, 
			     gpointer data)
{
  gpointer info[2];

  g_return_if_fail (curve != NULL);
  g_return_if_fail (func != NULL);

  /* forbid removal of edges */
  curve->keep_edges = TRUE;
  info[0] = func;
  info[1] = data;
  g_hash_table_foreach (curve->edges, (GHFunc) foreach_edge, info);
  /* allow removal of edges */
  curve->keep_edges = FALSE;
}

static void vertex_foreach_edge (GtsSegment * s,
				 gpointer ce_data, 
				 gpointer * info)
{
  GHashTable * hash = info[0];
  gpointer data = info[1];
  GtsFunc func = info[2];

  if (!g_hash_table_lookup (hash, s->v1)) {
    (*func) (s->v1, data);
    g_hash_table_insert (hash, s->v1, GINT_TO_POINTER (-1));
  }
  if (!g_hash_table_lookup (hash, s->v2)) {
    (*func) (s->v2, data);
    g_hash_table_insert (hash, s->v2, GINT_TO_POINTER (-1));
  }
}

void gts_curve_foreach_vertex (GtsCurve * curve,
			       GtsFunc func,
			       gpointer data)
{
  gpointer info[3];

  g_return_if_fail (curve != NULL);
  g_return_if_fail (func != NULL);
  
  /* forbid removal of edges */
  curve->keep_edges = TRUE;
  info[0] = g_hash_table_new (NULL, NULL);
  info[1] = data;
  info[2] = func;
  g_hash_table_foreach (curve->edges, 
			(GHFunc) vertex_foreach_edge, info);
  g_hash_table_destroy (info[0]);
  /* allow removal of faces */
  curve->keep_edges = FALSE;
}

typedef struct _GtsCurveInter         GtsCurveInter;
typedef struct _GtsCurveInterClass    GtsCurveInterClass;

struct _GtsCurveInter {
  GtsObject object;

  GtsCurve * c1, * c2;
  GSList * vertices;
};

struct _GtsCurveInterClass {
  GtsObjectClass parent_class;
};

#define GTS_CURVE_INTER(obj)            GTS_OBJECT_CAST (obj,\
					           GtsCurveInter,\
					           gts_curve_inter_class ())
#define GTS_CURVE_INTER_CLASS(klass)    GTS_OBJECT_CLASS_CAST (klass,\
						         GtsCurveInterClass,\
						         gts_curve_inter_class())
#define IS_GTS_CURVE_INTER(obj)         (gts_object_is_from_class (obj,\
						   gts_curve_inter_class ()))
     
GtsCurveInterClass * gts_curve_inter_class    (void);
GtsCurveInter *      gts_curve_inter_new      (GtsCurveInterClass * klass,
					       GtsCurve * c1,
					       GtsCurve * c2);

static void curve_inter_destroy (GtsObject * object)
{
  GtsCurveInter * ci = GTS_CURVE_INTER (object);

  gts_object_destroy (GTS_OBJECT (ci->c1));
  gts_object_destroy (GTS_OBJECT (ci->c2));
  g_slist_free (ci->vertices);

  (* GTS_OBJECT_CLASS (gts_curve_inter_class ())->parent_class->destroy) 
    (object);
}

static void curve_inter_class_init (GtsCurveInterClass * klass)
{
  GTS_OBJECT_CLASS (klass)->destroy = curve_inter_destroy;
}

static void curve_inter_init (GtsCurveInter * ci)
{
  ci->c1 = ci->c2 = NULL;
  ci->vertices = NULL;
}

GtsCurveInterClass * gts_curve_inter_class (void)
{
  static GtsCurveInterClass * klass = NULL;

  if (klass == NULL) {
    GtsObjectClassInfo curve_inter_info = {
      "GtsCurveInter",
      sizeof (GtsCurveInter),
      sizeof (GtsCurveInterClass),
      (GtsObjectClassInitFunc) curve_inter_class_init,
      (GtsObjectInitFunc) curve_inter_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
    };
    klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_object_class ()),
				  &curve_inter_info);
  }

  return klass;
}

void gts_delaunay_force_add_constraint (GtsSurface * s,
					GtsConstraint * c);

static void create_list (gpointer object, GSList ** list)
{
  *list = g_slist_prepend (*list, object);
}

/* returns TRUE if C is between A and B */
static gboolean is_between (GtsPoint * C, 
			    GtsPoint * A, GtsPoint * B,
			    GtsPoint * E)
{
  gdouble AEC = gts_point_orientation (A, E, C);
  gdouble BEC = gts_point_orientation (B, E, C);
  
  if ((AEC >= 0.0 && BEC <= 0.0) || (AEC < 0.0 && BEC > 0.0))
    return TRUE;
  return FALSE;
}

static GtsPoint * segment_intersection (GtsVertexClass * klass,
					GtsSegment * s1,
					GtsSegment * s2)
{
  GtsPoint * A, * B, * C, * D, * I = NULL;
  gdouble ABD, ABC, CDA, CDB;
  gdouble c;

  A = GTS_POINT (s1->v1);
  B = GTS_POINT (s1->v2);
  C = GTS_POINT (s2->v1);
  D = GTS_POINT (s2->v2);

  ABC = gts_point_orientation (A, B, C);
  ABD = gts_point_orientation (A, B, D);
  
  if (ABD < 0.0 || ABC > 0.0) {
    GtsPoint * tmpp;
    gdouble tmp;
    tmpp = D; D = C; C = tmpp;
    tmp = ABC; ABC = ABD; ABD = tmp;
  }
  if (ABD < 0.0 || ABC > 0.0)
    return NULL;
  CDB = gts_point_orientation (C, D, B);
  if (CDB > 0.0)
    return NULL;
  CDA = gts_point_orientation (C, D, A);
  if (CDA < 0.0)
    return NULL;
  if (ABD == 0.0) {
    if (ABC == 0.0) {
      /* s1 and s2 are colinear */
      GtsVector Z = { 0., 0., 1. }, AB, AE;
      GtsPoint * E;

      gts_vector_init (AB, A, B);
      gts_vector_cross (AE, Z, AB);
      E = gts_point_new (gts_point_class (), 
			 A->x + AE[0], A->y + AE[1], A->z + AE[2]);
      g_assert (gts_point_orientation (A, B, E) > 0.0);

      if (is_between (C, A, B, E)) I = C;
      else if (is_between (D, A, B, E)) I = D;
      else if (is_between (A, C, D, E)) I = A;
      else if (is_between (B, C, D, E)) I = B;

      gts_object_destroy (GTS_OBJECT (E));
      return I;
    }
    return D;
  }
  if (ABC == 0.0)
    return C;
  if (CDB == 0.0)
    return B;
  if (CDA == 0.0)
    return A;
  c = ABC/(ABC - ABD);
  I = GTS_POINT (gts_object_new (GTS_OBJECT_CLASS (klass)));
  gts_point_set (I, 
		 C->x + c*(D->x - C->x),
		 C->y + c*(D->y - C->y),
		 C->z);
  return I;
}

static void print_segment (GtsSegment * s, FILE * fp)
{
  fprintf (fp, "%p: (%g,%g)->(%g,%g)",
	   s, 
	   GTS_POINT (s->v1)->x, GTS_POINT (s->v1)->y,
	   GTS_POINT (s->v2)->x, GTS_POINT (s->v2)->y);
}

static void segment_detach (GtsSegment * s)
{
  s->v1->segments = g_slist_remove (s->v1->segments, s);
  s->v2->segments = g_slist_remove (s->v2->segments, s);
  s->v1 = s->v2 = NULL;
}

static void segment_attach (GtsSegment * s, GtsVertex * v1, GtsVertex * v2)
{
  s->v1 = v1;
  v1->segments = g_slist_prepend (v1->segments, s);
  s->v2 = v2;
  v2->segments = g_slist_prepend (v2->segments, s);
}

static GtsConstraint * new_constraint (GtsVertex * v1,
				       GtsVertex * v2,
				       GtsConstraint * c)
{
  GtsSegment * s;

  if (v1 == v2)
    return NULL;

  s = gts_vertices_are_connected (v1, v2);
  if (GTS_SEGMENT (c)->v1 == NULL)
    segment_attach (GTS_SEGMENT (c), v1, v2);
  else {
    GtsConstraint * c1 = 
      GTS_CONSTRAINT (gts_edge_new (GTS_EDGE_CLASS (GTS_OBJECT (c)->klass), 
				    v1, v2));
    if (GTS_IS_CEDGE (c))
      g_slist_foreach (GTS_CEDGE (c)->curves, (GFunc) gts_curve_add_edge, c1);
    c = c1;
  }
  if (s != NULL && GTS_IS_EDGE (s)) {
    /*    g_assert (!GTS_IS_CONSTRAINT (s)); */
    if (GTS_IS_CEDGE (s))
      gts_cedge_replace (GTS_CEDGE (s), GTS_CEDGE (c));
    else
      gts_edge_replace (GTS_EDGE (s), GTS_EDGE (c));
    gts_object_destroy (GTS_OBJECT (s));
  }
  return c;
}

static GSList * delaunay_add_unencroached_constraint (GtsSurface * s,
						      GtsConstraint * c)
{
  GSList * conflict = gts_delaunay_add_constraint (s, c);
  GSList * i = GTS_EDGE (c)->triangles;
  gboolean encroached = FALSE;
  GtsVertex * v1 = GTS_SEGMENT (c)->v1;
  GtsVertex * v2 = GTS_SEGMENT (c)->v2;
  GtsVector Z = { 0., 0., 1. }, AB, AE;
  GtsPoint * E;
  
  gts_vector_init (AB, GTS_POINT (v1), GTS_POINT (v2));
  gts_vector_cross (AE, Z, AB);
  E = gts_point_new (gts_point_class (), 
		     GTS_POINT (v1)->x + AE[0], 
		     GTS_POINT (v1)->y + AE[1], 
		     GTS_POINT (v1)->z + AE[2]);
  g_assert (gts_point_orientation (GTS_POINT (v1), GTS_POINT (v2), E) > 0.0);

  while (i && !encroached) {
    GtsFace * f = i->data;
    if (GTS_IS_FACE (f) && gts_face_has_parent_surface (f, s)) {
      GtsVertex * v = gts_triangle_vertex_opposite (GTS_TRIANGLE (f), 
						    GTS_EDGE (c));
      if (gts_point_orientation (GTS_POINT (v1), 
				 GTS_POINT (v2), 
				 GTS_POINT (v)) == 0. &&
	  is_between (GTS_POINT (v), GTS_POINT (v1), GTS_POINT (v2), E)) {
	GtsEdge * e = gts_edge_new (s->edge_class, v1, v2);
	GSList * j = conflict;
	GtsConstraint * c1;

	gts_edge_replace (GTS_EDGE (c), e);
	segment_detach (GTS_SEGMENT (c));
	
	while (j) {
	  g_assert (gts_delaunay_add_constraint (s, j->data) == NULL);
	  j = j->next;
	}
	g_slist_free (conflict);
	conflict = NULL;
	encroached = TRUE;
	
	if ((c1 = new_constraint (v1, v, c)))
	  gts_delaunay_force_add_constraint (s, c1);
	if ((c1 = new_constraint (v, v2, c)))
	  gts_delaunay_force_add_constraint (s, c1);
      }
    }
    i = i->next;
  }

  gts_object_destroy (GTS_OBJECT (E));
  return conflict;
}

void gts_delaunay_force_add_constraint (GtsSurface * s,
					GtsConstraint * c)
{
  GSList * conflict;

  g_return_if_fail (s != NULL);
  g_return_if_fail (c != NULL);

  {
    FILE * fp = fopen ("/tmp/toto", "wt");
    gts_surface_write_oogl (s, fp);
    fclose (fp);
    fprintf (stderr, "Adding constraint "); 
    print_segment (GTS_SEGMENT (c), stderr);
    fprintf (stderr, "\n");
  }

  if ((conflict = delaunay_add_unencroached_constraint (s, c))) {
    GtsConstraint * c1 = conflict->data;
    GSList * i = conflict->next;
    GtsVertex * v1, * v2, * v3, * v4, * vi, * dup;
    GtsConstraint * c2;
    GtsEdge * e;

  {
    FILE * fp = fopen ("/tmp/toto", "wt");
    gts_surface_write_oogl (s, fp);
    fclose (fp);
  }
gts_surface_print_stats (s, stderr);
    
    v1 = GTS_SEGMENT (c)->v1; v2 = GTS_SEGMENT (c)->v2;
    v3 = GTS_SEGMENT (c1)->v1; v4 = GTS_SEGMENT (c1)->v2;
    vi = GTS_VERTEX (segment_intersection (s->vertex_class,
					   GTS_SEGMENT (c),
					   GTS_SEGMENT (c1)));
    g_assert (vi);

    e = gts_edge_new (s->edge_class, GTS_SEGMENT (c)->v1, GTS_SEGMENT (c)->v2);
    gts_edge_replace (GTS_EDGE (c), e);
    segment_detach (GTS_SEGMENT (c));
    g_assert (GTS_EDGE (c1)->triangles == NULL);
    segment_detach (GTS_SEGMENT (c1));
    
    while (i) {
      g_assert (gts_delaunay_add_constraint (s, i->data) == NULL);
      i = i->next;
    }
    g_slist_free (conflict);

gts_surface_print_stats (s, stderr);
fprintf (stderr, "adding vertex (%g,%g)\n", 
	 GTS_POINT (vi)->x,
	 GTS_POINT (vi)->y);
    if ((dup = gts_delaunay_add_vertex (s, vi, NULL))) {
      gts_object_destroy (GTS_OBJECT (vi));
      vi = dup;
    }

gts_surface_print_stats (s, stderr);

    if ((c2 = new_constraint (v1, vi, c)))
      gts_delaunay_force_add_constraint (s, c2);
    if ((c2 = new_constraint (vi, v2, c)))
      gts_delaunay_force_add_constraint (s, c2);
    if ((c2 = new_constraint (v3, vi, c1)))
      gts_delaunay_force_add_constraint (s, c2);
    if ((c2 = new_constraint (vi, v4, c1)))
      gts_delaunay_force_add_constraint (s, c2);
  }
  else
gts_surface_print_stats (s, stderr);    
}

static void vertex_replace_cleanup (GtsVertex * v, GtsVertex * with)
{
  GSList * i;

  g_return_if_fail (v != NULL);
  g_return_if_fail (with != NULL);
  g_return_if_fail (v != with);

  gts_vertex_replace (v, with);

  i = with->segments;
  while (i) {
    GtsSegment * s = i->data;
    GSList * next = i->next;
    GtsSegment * dup;
    
    g_assert (GTS_IS_EDGE (s));
    while ((dup = gts_segment_is_duplicate (s))) {
      g_assert (GTS_IS_EDGE (dup));
      if (GTS_IS_CEDGE (s)) {
	if (GTS_IS_CEDGE (dup))
	  gts_cedge_replace (GTS_CEDGE (dup), GTS_CEDGE (s));
	else
	  gts_edge_replace (GTS_EDGE (dup), GTS_EDGE (s));
	gts_object_destroy (GTS_OBJECT (dup));
	next = i->next;
      }
      else {
	gts_edge_replace (GTS_EDGE (s), GTS_EDGE (dup));
	gts_object_destroy (GTS_OBJECT (s));
	s = dup;
      }
    }
    i = next;
  }
}

gboolean gts_delaunay_force_add_vertex (GtsSurface * surface, GtsVertex * v)
{
  GtsVertex * dup;

  g_return_val_if_fail (surface != NULL, FALSE);
  g_return_val_if_fail (v != NULL, FALSE);

  dup = gts_delaunay_add_vertex (surface, v, NULL);
  if (dup == NULL)
    return TRUE;
  if (dup == v)
    return FALSE;
  vertex_replace_cleanup (v, dup);
  gts_object_destroy (GTS_OBJECT (v));
  return TRUE;
}

static void read_curve (GtsCurve * c, FILE * fp)
{
  guint nv, ns, i = 0, v1, v2;
  GtsVertex ** vertices;
  gdouble x, y;

  g_return_if_fail (c != NULL);
  g_return_if_fail (fp != NULL);

  if (fscanf (fp, "%u %u", &nv, &ns) != 2)
    return;

  vertices = g_malloc (sizeof (GtsVertex *)*nv);
  while (i < nv && fscanf (fp, "%lf %lf", &x, &y) == 2)
    vertices[i++] = gts_vertex_new (c->vertex_class, x, y, 0.);
  i = 0;
  while (i < ns && fscanf (fp, "%u %u", &v1, &v2) == 2) {
    g_return_if_fail (v1 <= nv && v2 <= nv);
    gts_curve_add_edge (c, gts_cedge_new (c->cedge_class, 
					  vertices[v1 - 1], 
					  vertices[v2 - 1]),
			GTS_DIRECT);
    i++;
  }
  g_free (vertices);
}

static void write_edge (GtsSegment * s, FILE * fp)
{
  fprintf (fp, "%g %g\n%g %g\n\n",
	   GTS_POINT (s->v1)->x,
	   GTS_POINT (s->v1)->y,
	   GTS_POINT (s->v2)->x,
	   GTS_POINT (s->v2)->y);
}

static void write_curve (GtsCurve * c, FILE * fp)
{
  g_return_if_fail (c != NULL);
  g_return_if_fail (fp != NULL);

  gts_curve_foreach_edge (c, (GtsFunc) write_edge, fp);
}

int main (int argc, char * argv[])
{
  GtsCurve * c1, * c2;
  GtsSurface * s;
  FILE * fp1, * fp2;
  GSList * vertices = NULL;
  GSList * constraints = NULL;
  GtsTriangle * t;
  GtsVertex * v1, * v2, * v3;
  GSList * i;

  if (argc != 3) {
    fprintf (stderr, "usage: clipping FILE1 FILE2\n");
    return 1;
  }

  c1 = gts_curve_new (gts_curve_class (),
		      gts_cedge_class (),
		      gts_vertex_class ());
  fp1 = fopen (argv[1], "rt");
  if (fp1 == NULL) {
    fprintf (stderr, "clipping: cannot open file `%s'\n", argv[1]);
    return 1;
  }
  read_curve (c1, fp1);
  fclose (fp1);

  c2 = gts_curve_new (gts_curve_class (),
		      gts_cedge_class (),
		      gts_vertex_class ());
  fp2 = fopen (argv[2], "rt");
  if (fp2 == NULL) {
    fprintf (stderr, "clipping: cannot open file `%s'\n", argv[2]);
    return 1;
  }
  read_curve (c2, fp2);
  fclose (fp2);  

  s = gts_surface_new (gts_surface_class (),
		       gts_face_class (),
		       gts_edge_class (),
		       gts_vertex_class ());
  gts_curve_foreach_vertex (c1, (GtsFunc) create_list, &vertices);
  gts_curve_foreach_vertex (c2, (GtsFunc) create_list, &vertices);

  t = gts_triangle_enclosing (gts_triangle_class (), vertices, 1.1);
  gts_surface_add_face (s, gts_face_new (gts_face_class (),
					 t->e1, t->e2, t->e3));
  gts_triangle_vertices (t, &v1, &v2, &v3);
  gts_object_destroy (GTS_OBJECT (t));

  i = vertices;
  while (i) {
    gts_delaunay_force_add_vertex (s, i->data);
    i = i->next;
  }

  gts_curve_foreach_edge (c1, (GtsFunc) create_list, &constraints);
  gts_curve_foreach_edge (c2, (GtsFunc) create_list, &constraints);
  i = constraints;
  while (i) {
    gts_delaunay_force_add_constraint (s, i->data);
    i = i->next;
  }

  gts_allow_floating_vertices = TRUE;
  gts_object_destroy (GTS_OBJECT (v1));
  gts_object_destroy (GTS_OBJECT (v2));
  gts_object_destroy (GTS_OBJECT (v3));
  gts_allow_floating_vertices = FALSE;

  gts_surface_print_stats (s, stderr);
  gts_surface_write (s, stdout);

  fp1 = fopen ("/tmp/c1", "wt");
  write_curve (c1, fp1);
  fclose (fp1);

  fp2 = fopen ("/tmp/c2", "wt");
  write_curve (c2, fp2);
  fclose (fp2);

  return 0;
}
