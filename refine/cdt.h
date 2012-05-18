#ifndef __P2TC_REFINE_CDT_H__
#define __P2TC_REFINE_CDT_H__

#include <p2t/poly2tri.h>
#include "mesh.h"
#include "pslg.h"

typedef struct
{
  P2trMesh *mesh;
  P2trPSLG *outline;
} P2trCDT;

void        p2tr_cdt_validate_unused (P2trCDT* self);

/**
 * Create a new P2trCDT from an existing P2tCDT. The resulting P2trCDT
 * does not depend on the original P2tCDT which can be freed
 * @param cdt A P2tCDT Constrained Delaunay Triangulation
 * @return A P2trCDT Constrained Delaunay Triangulation
 */
P2trCDT*    p2tr_cdt_new  (P2tCDT *cdt);

void        p2tr_cdt_free (P2trCDT *cdt);

/**
 * Test whether there is a path from the point @ref p to the edge @e
 * so that the path does not cross any segment of the CDT
 */
gboolean    p2tr_cdt_visible_from_edge (P2trCDT     *self,
                                        P2trEdge    *e,
                                        P2trVector2 *p);

/**
 * Make sure that all edges either have two triangles (one on each side)
 * or that they are constrained. The reason for that is that the
 * triangulation domain of the CDT is defined by a closed PSLG.
 */
void        p2tr_cdt_validate_edges    (P2trCDT *self);

/**
 * Make sure the constrained empty circum-circle property holds,
 * meaning that each triangles circum-scribing circle is either empty
 * or only contains points which are not "visible" from the edges of
 * the triangle.
 */
void        p2tr_cdt_validate_cdt      (P2trCDT *self);

/**
 * Insert a point into the triangulation while preserving the
 * constrained delaunay property
 * @param self The CDT into which the point should be inserted
 * @param pc The point to insert
 * @param point_location_guess A triangle which may be near the
 *        area containing the point (or maybe even contains the point).
 *        The better the guess is, the faster the insertion will be. If
 *        such a triangle is unknown, NULL may be passed.
 */
P2trPoint*  p2tr_cdt_insert_point      (P2trCDT           *self,
                                        const P2trVector2 *pc,
                                        P2trTriangle      *point_location_guess);

/**
 * Similar to @ref p2tr_cdt_insert_point, but assumes that the point to
 * insert is located inside the area of the given triangle
 */
void        p2tr_cdt_insert_point_into_triangle (P2trCDT      *self,
                                                 P2trPoint    *pt,
                                                 P2trTriangle *tri);

/**
 * Insert a point so that is splits an existing edge, while preserving
 * the constrained delaunay property. This function assumes that the
 * point is on the edge itself and between its end-points.
 * If the edge being split is constrained, then the function returns a
 * list containing both parts resulted from the splitting. In that case,
 * THE RETURNED EDGES MUST BE UNREFERENCED!
 */
GList*      p2tr_cdt_split_edge (P2trCDT   *self,
                                 P2trEdge  *e,
                                 P2trPoint *C);

#endif
