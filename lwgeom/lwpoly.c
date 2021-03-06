/**********************************************************************
 *
 * PostGIS - Spatial Types for PostgreSQL
 * http://postgis.refractions.net
 *
 * Copyright (C) 2012 Sandro Santilli <strk@keybit.net>
 * Copyright (C) 2001-2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU General Public Licence. See the COPYING file.
 *
 **********************************************************************/

/* basic LWPOLY manipulation */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liblwgeom_internal.h"
#include "lwgeom_log.h"


#define CHECK_POLY_RINGS_ZM 1

/* construct a new LWPOLY.  arrays (points/points per ring) will NOT be copied
 * use SRID=SRID_UNKNOWN for unknown SRID (will have 8bit type's S = 0)
 */
LWPOLY*
lwpoly_construct(int srid, GBOX *bbox, uint32_t nrings, POINTARRAY **points)
{
	LWPOLY *result;
	int hasz, hasm;
#ifdef CHECK_POLY_RINGS_ZM
	char zm;
	uint32_t i;
#endif

	if ( nrings < 1 ) lwerror("lwpoly_construct: need at least 1 ring");

	hasz = FLAGS_GET_Z(points[0]->flags);
	hasm = FLAGS_GET_M(points[0]->flags);

#ifdef CHECK_POLY_RINGS_ZM
	zm = FLAGS_GET_ZM(points[0]->flags);
	for (i=1; i<nrings; i++)
	{
		if ( zm != FLAGS_GET_ZM(points[i]->flags) )
			lwerror("lwpoly_construct: mixed dimensioned rings");
	}
#endif

	result = (LWPOLY*) lwalloc(sizeof(LWPOLY));
	result->type = POLYGONTYPE;
	result->flags = gflags(hasz, hasm, 0);
	FLAGS_SET_BBOX(result->flags, bbox?1:0);
	result->srid = srid;
	result->nrings = nrings;
	result->maxrings = nrings;
	result->rings = points;
	result->bbox = bbox;

	return result;
}

LWPOLY*
lwpoly_construct_empty(int srid, char hasz, char hasm)
{
	LWPOLY *result = (LWPOLY *)lwalloc(sizeof(LWPOLY));
	result->type = POLYGONTYPE;
	result->flags = gflags(hasz,hasm,0);
	result->srid = srid;
	result->nrings = 0;
	result->maxrings = 1; /* Allocate room for ring, just in case. */
	result->rings = (POINTARRAY **)lwalloc(result->maxrings * sizeof(POINTARRAY*));
	result->bbox = NULL;
	return result;
}

void lwpoly_free(LWPOLY  *poly)
{
	int t;

	if( ! poly ) return;

	if ( poly->bbox )
		lwfree(poly->bbox);

	for (t=0; t<poly->nrings; t++)
	{
		if ( poly->rings[t] )
			ptarray_free(poly->rings[t]);
	}

	if ( poly->rings )
		lwfree(poly->rings);

	lwfree(poly);
}

void printLWPOLY(LWPOLY *poly)
{
	int t;
	lwnotice("LWPOLY {");
	lwnotice("    ndims = %i", (int)FLAGS_NDIMS(poly->flags));
	lwnotice("    SRID = %i", (int)poly->srid);
	lwnotice("    nrings = %i", (int)poly->nrings);
	for (t=0; t<poly->nrings; t++)
	{
		lwnotice("    RING # %i :",t);
		printPA(poly->rings[t]);
	}
	lwnotice("}");
}

/* @brief Clone LWLINE object. Serialized point lists are not copied.
 *
 * @see ptarray_clone 
 */
LWPOLY *
lwpoly_clone(const LWPOLY *g)
{
	int i;
	LWPOLY *ret = (LWPOLY *)lwalloc(sizeof(LWPOLY));
	memcpy(ret, g, sizeof(LWPOLY));
	ret->rings = (POINTARRAY **)lwalloc(sizeof(POINTARRAY *)*g->nrings);
	for ( i = 0; i < g->nrings; i++ ) {
		ret->rings[i] = ptarray_clone(g->rings[i]);
	}
	if ( g->bbox ) ret->bbox = gbox_copy(g->bbox);
	return ret;
}

/* Deep clone LWPOLY object. POINTARRAY are copied, as is ring array */
LWPOLY *
lwpoly_clone_deep(const LWPOLY *g)
{
	int i;
	LWPOLY *ret = (LWPOLY *)lwalloc(sizeof(LWPOLY));
	memcpy(ret, g, sizeof(LWPOLY));
	if ( g->bbox ) ret->bbox = gbox_copy(g->bbox);
	ret->rings = (POINTARRAY **)lwalloc(sizeof(POINTARRAY *)*g->nrings);
	for ( i = 0; i < ret->nrings; i++ )
	{
		ret->rings[i] = ptarray_clone_deep(g->rings[i]);
	}
	FLAGS_SET_READONLY(ret->flags,0);
	return ret;
}

/**
* Add a ring to a polygon. Point array will be referenced, not copied.
*/
int
lwpoly_add_ring(LWPOLY *poly, POINTARRAY *pa) 
{
	if( ! poly || ! pa ) 
		return LW_FAILURE;
		
	/* We have used up our storage, add some more. */
	if( poly->nrings >= poly->maxrings ) 
	{
		int new_maxrings = 2 * (poly->nrings + 1);
		poly->rings = (POINTARRAY **)lwrealloc(poly->rings, new_maxrings * sizeof(POINTARRAY*));
	}
	
	/* Add the new ring entry. */
	poly->rings[poly->nrings] = pa;
	poly->nrings++;
	
	return LW_SUCCESS;
}

void
lwpoly_force_clockwise(LWPOLY *poly)
{
	int i;

	/* No-op empties */
	if ( lwpoly_is_empty(poly) )
		return;

	/* External ring */
	if ( ptarray_isccw(poly->rings[0]) )
		ptarray_reverse(poly->rings[0]);

	/* Internal rings */
	for (i=1; i<poly->nrings; i++)
		if ( ! ptarray_isccw(poly->rings[i]) )
			ptarray_reverse(poly->rings[i]);

}

void
lwpoly_release(LWPOLY *lwpoly)
{
	lwgeom_release(lwpoly_as_lwgeom(lwpoly));
}

void
lwpoly_reverse(LWPOLY *poly)
{
	int i;
	if ( lwpoly_is_empty(poly) ) return;
	for (i=0; i<poly->nrings; i++)
		ptarray_reverse(poly->rings[i]);
}

LWPOLY *
lwpoly_segmentize2d(LWPOLY *poly, double dist)
{
	POINTARRAY **newrings;
	uint32_t i;

	newrings = (POINTARRAY **)lwalloc(sizeof(POINTARRAY *)*poly->nrings);
	for (i=0; i<poly->nrings; i++)
	{
		newrings[i] = ptarray_segmentize2d(poly->rings[i], dist);
	}
	return lwpoly_construct(poly->srid, NULL,
	                        poly->nrings, newrings);
}

/*
 * check coordinate equality
 * ring and coordinate order is considered
 */
char
lwpoly_same(const LWPOLY *p1, const LWPOLY *p2)
{
	uint32_t i;

	if ( p1->nrings != p2->nrings ) return 0;
	for (i=0; i<p1->nrings; i++)
	{
		if ( ! ptarray_same(p1->rings[i], p2->rings[i]) )
			return 0;
	}
	return 1;
}

/*
 * Construct a polygon from a LWLINE being
 * the shell and an array of LWLINE (possibly NULL) being holes.
 * Pointarrays from intput geoms are cloned.
 * SRID must be the same for each input line.
 * Input lines must have at least 4 points, and be closed.
 */
LWPOLY *
lwpoly_from_lwlines(const LWLINE *shell,
                    uint32_t nholes, const LWLINE **holes)
{
	uint32_t nrings;
	POINTARRAY **rings = (POINTARRAY **)lwalloc((nholes+1)*sizeof(POINTARRAY *));
	int srid = shell->srid;
	LWPOLY *ret;

	if ( shell->points->npoints < 4 )
		lwerror("lwpoly_from_lwlines: shell must have at least 4 points");
	if ( ! ptarray_isclosed2d(shell->points) )
		lwerror("lwpoly_from_lwlines: shell must be closed");
	rings[0] = ptarray_clone_deep(shell->points);

	for (nrings=1; nrings<=nholes; nrings++)
	{
		const LWLINE *hole = holes[nrings-1];

		if ( hole->srid != srid )
			lwerror("lwpoly_from_lwlines: mixed SRIDs in input lines");

		if ( hole->points->npoints < 4 )
			lwerror("lwpoly_from_lwlines: holes must have at least 4 points");
		if ( ! ptarray_isclosed2d(hole->points) )
			lwerror("lwpoly_from_lwlines: holes must be closed");

		rings[nrings] = ptarray_clone_deep(hole->points);
	}

	ret = lwpoly_construct(srid, NULL, nrings, rings);
	return ret;
}

LWGEOM*
lwpoly_remove_repeated_points(LWPOLY *poly)
{
	uint32_t i;
	POINTARRAY **newrings;

	newrings = (POINTARRAY **)lwalloc(sizeof(POINTARRAY *)*poly->nrings);
	for (i=0; i<poly->nrings; i++)
	{
		newrings[i] = ptarray_remove_repeated_points(poly->rings[i]);
	}

	return (LWGEOM*)lwpoly_construct(poly->srid,
	                                 poly->bbox ? gbox_copy(poly->bbox) : NULL,
	                                 poly->nrings, newrings);

}


LWPOLY*
lwpoly_force_dims(const LWPOLY *poly, int hasz, int hasm)
{
	LWPOLY *polyout;
	
	/* Return 2D empty */
	if( lwpoly_is_empty(poly) )
	{
		polyout = lwpoly_construct_empty(poly->srid, hasz, hasm);
	}
	else
	{
		POINTARRAY **rings = NULL;
		int i;
		rings = (POINTARRAY **)lwalloc(sizeof(POINTARRAY*) * poly->nrings);
		for( i = 0; i < poly->nrings; i++ )
		{
			rings[i] = ptarray_force_dims(poly->rings[i], hasz, hasm);
		}
		polyout = lwpoly_construct(poly->srid, NULL, poly->nrings, rings);
	}
	polyout->type = poly->type;
	return polyout;
}

int lwpoly_is_empty(const LWPOLY *poly)
{
	if ( (poly->nrings < 1) || (!poly->rings) || (!poly->rings[0]) || (poly->rings[0]->npoints < 1) )
		return LW_TRUE;
	return LW_FALSE;
}

int lwpoly_count_vertices(LWPOLY *poly)
{
	int i = 0;
	int v = 0; /* vertices */
	assert(poly);
	for ( i = 0; i < poly->nrings; i ++ )
	{
		v += poly->rings[i]->npoints;
	}
	return v;
}

LWPOLY* lwpoly_simplify(const LWPOLY *ipoly, double dist)
{
	int i;
	LWPOLY *opoly = lwpoly_construct_empty(ipoly->srid, FLAGS_GET_Z(ipoly->flags), FLAGS_GET_M(ipoly->flags));

	LWDEBUGF(2, "simplify_polygon3d: simplifying polygon with %d rings", ipoly->nrings);

	if( lwpoly_is_empty(ipoly) )
		return opoly; /* should we return NULL instead ? */

	for (i = 0; i < ipoly->nrings; i++)
	{
		static const int minvertices = 0; /* TODO: allow setting this */
		POINTARRAY *opts = ptarray_simplify(ipoly->rings[i], dist, minvertices);

		LWDEBUGF(3, "ring%d simplified from %d to %d points", i, ipoly->rings[i]->npoints, opts->npoints);

		/* Less points than are needed to form a closed ring, we can't use this */
		if ( opts->npoints < 4 )
		{
			LWDEBUGF(3, "ring%d skipped (% pts)", i, opts->npoints);
			ptarray_free(opts);
			if ( i ) continue;
			else break; /* Don't scan holes if shell is collapsed */
		}

		/* Add ring to simplified polygon */
		if( lwpoly_add_ring(opoly, opts) == LW_FAILURE )
			return NULL;
	}

	LWDEBUGF(3, "simplified polygon with %d rings", ipoly->nrings);
	opoly->type = ipoly->type;

	if( lwpoly_is_empty(opoly) )
		return NULL;

	return opoly;
}

/**
 * Find the area of the outer ring - sum (area of inner rings).
 * Could use a more numerically stable calculator...
 */
double
lwpoly_area(const LWPOLY *poly)
{
	double poly_area=0.0;
	int i;
	POINT2D pp;
	POINT2D cp;
	POINT2D np;
        double x0 = cp.x;

	LWDEBUGF(2, "in lwpoly_area (%d rings)", poly->nrings);

	for (i=0; i<poly->nrings; i++)
	{
		int j;
		POINTARRAY *ring = poly->rings[i];
		double ringarea = 0.0;

		LWDEBUGF(4, " rings %d has %d points", i, ring->npoints);

		if ( ! ring->npoints ) continue; /* empty ring */

		getPoint2d_p(ring, 0, &cp);
		getPoint2d_p(ring, 1, &np);
                x0 = cp.x;
                np.x -= x0;
                for (j=0; j<ring->npoints-1; j++)
		{
                        pp.y = cp.y;
                        cp.x = np.x;
                        cp.y = np.y;
			getPoint2d_p(ring, j+1, &np);
                        np.x -= x0;
                        ringarea += cp.x * (np.y - pp.y);
		}

		ringarea  /= 2.0;

		LWDEBUGF(4, " ring 1 has area %lf",ringarea);

		ringarea  = fabs(ringarea);
		if (i != 0)	/*outer */
			ringarea  = -1.0*ringarea ; /* its a hole */

		poly_area += ringarea;
	}

	return poly_area;
}


/**
 * Compute the sum of polygon rings length.
 * Could use a more numerically stable calculator...
 */
double
lwpoly_perimeter(const LWPOLY *poly)
{
	double result=0.0;
	int i;

	LWDEBUGF(2, "in lwgeom_polygon_perimeter (%d rings)", poly->nrings);

	for (i=0; i<poly->nrings; i++)
		result += ptarray_length(poly->rings[i]);

	return result;
}

/**
 * Compute the sum of polygon rings length (forcing 2d computation).
 * Could use a more numerically stable calculator...
 */
double
lwpoly_perimeter_2d(const LWPOLY *poly)
{
	double result=0.0;
	int i;

	LWDEBUGF(2, "in lwgeom_polygon_perimeter (%d rings)", poly->nrings);

	for (i=0; i<poly->nrings; i++)
		result += ptarray_length_2d(poly->rings[i]);

	return result;
}

int
lwpoly_is_closed(const LWPOLY *poly)
{
	int i = 0;
	
	if ( poly->nrings == 0 ) 
		return LW_TRUE;
		
	for ( i = 0; i < poly->nrings; i++ )
	{
		if (FLAGS_GET_Z(poly->flags))
		{
			if ( ! ptarray_isclosed3d(poly->rings[i]) )
				return LW_FALSE;
		}
		else
		{	
			if ( ! ptarray_isclosed2d(poly->rings[i]) )
				return LW_FALSE;
		}
	}
	
	return LW_TRUE;
}



