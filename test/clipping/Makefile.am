## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src -I$(includedir)\
	 -DG_LOG_DOMAIN=\"Gts-test\"
LDADD = $(top_builddir)/src/libgts.la -lm
DEPS = $(top_builddir)/src/libgts.la

check_PROGRAMS = clipping

TESTS = \
	clipping_c1_c2.sh \
	clipping_c1_c4.sh \
	clipping_c1_c7.sh \
	clipping_l1_l2.sh \
	clipping_l1_l3.sh \
## two squares touching at one vertex
	clipping_c1_c5.sh \
## two squares touching at one edge
	clipping_c1_c6.sh \
## two colinear segments
	clipping_l1_l5.sh \
## two squares with coincident edges
	clipping_c1_c3.sh \
## a square and a rectangle touching at one edge
	clipping_c1_c8.sh \
## a square and a rectangle touching at one edge and one vertex
	clipping_c1_c9.sh \
## two colinear segments
	clipping_l1_l4.sh

XFAIL_TESTS = 

EXTRA_DIST = \
	c1 c2 c3 c4 c5 c6 c7 c8 c9 \
	l1 l2 l3 l4 l5 \
	clipping.sh \
	$(TESTS)

test-cleanup:
	rm -f errors

check-TESTS: test-cleanup
