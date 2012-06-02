/*
 * This file is a part of Poly2Tri-C
 * (c) Barak Itkin <lightningismyname@gmail.com>
 * http://code.google.com/p/poly2tri-c/
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of Poly2Tri nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __P2TC_REFINE_MESH_H__
#define __P2TC_REFINE_MESH_H__

#include <glib.h>
#include "vector2.h"
#include "utils.h"
#include "triangulation.h"

struct P2trMesh_
{
  P2trHashSet *triangles;
  P2trHashSet *edges;
  P2trHashSet *points;
  
  guint        refcount;
};

P2trMesh*     p2tr_mesh_new             (void);

P2trPoint*    p2tr_mesh_new_point       (P2trMesh          *mesh,
                                         const P2trVector2 *c);

P2trPoint*    p2tr_mesh_new_point2      (P2trMesh  *mesh,
                                         gdouble    x,
                                         gdouble    y);

P2trEdge*     p2tr_mesh_new_edge        (P2trMesh  *mesh,
                                         P2trPoint *start,
                                         P2trPoint *end,
                                         gboolean   constrained);

/**
 * Return a new edge between the two points if an edge doesn't exist, or
 * return an existing edge between the two points if there is such one.
 * THE RETURNED EDGE MUST BE UNREFFED, NO MATTER IF IT'S A NEW EDGE OR
 * AN EXISTING EDGE!
 */
P2trEdge*     p2tr_mesh_new_or_existing_edge (P2trMesh  *self,
                                              P2trPoint *start,
                                              P2trPoint *end,
                                              gboolean   constrained);

P2trTriangle* p2tr_mesh_new_triangle        (P2trMesh *mesh,
                                             P2trEdge *AB,
                                             P2trEdge *BC,
                                             P2trEdge *CA);

void          p2tr_mesh_on_point_removed    (P2trMesh  *mesh,
                                             P2trPoint *point);

void          p2tr_mesh_on_edge_removed     (P2trMesh *mesh,
                                             P2trEdge *edge);

void          p2tr_mesh_on_triangle_removed (P2trMesh     *mesh,
                                             P2trTriangle *triangle);

void          p2tr_mesh_clear           (P2trMesh *mesh);

void          p2tr_mesh_destroy         (P2trMesh *mesh);

void          p2tr_mesh_unref           (P2trMesh *mesh);

P2trMesh*     p2tr_mesh_ref             (P2trMesh *mesh);

P2trTriangle* p2tr_mesh_find_point      (P2trMesh *self,
                                         const P2trVector2 *pt);

P2trTriangle* p2tr_mesh_find_point2     (P2trMesh          *self,
                                         const P2trVector2 *pt,
                                         gdouble           *u,
                                         gdouble           *v);

/** This function assumes the mesh is composed entirely of one
 *  continuous region. The region may have holes, but eventually every
 *  triangle should be accessible from any other triangle by going
 *  through a chain of neigbor triangles
 * @param[in] self The mesh to search
 * @param[in] pt The point to find
 * @param[in] initial_guess An initial guess for which triangle contains
 *            the point or is at least near it
 * @return The triangle containing the point, or NULL if it's outside
 *         the triangulation domain
 */
P2trTriangle* p2tr_mesh_find_point_local (P2trMesh *self,
                                          const P2trVector2 *pt,
                                          P2trTriangle *initial_guess);

/** Same as @ref p2tr_mesh_find_point_local but also returns the u and v
 * coordinates of the given point inside the triangle
 * @param[in] self The mesh to search
 * @param[in] pt The point to find
 * @param[in] initial_guess An initial guess for which triangle contains
 *            the point or is at least near it
 * @param[out] u The u coordinate of the point inside the returned triangle
 * @param[out] v The v coordinate of the point inside the returned triangle
 * @return The triangle containing the point, or NULL if it's outside
 *         the triangulation domain
 */
P2trTriangle* p2tr_mesh_find_point_local2 (P2trMesh *self,
                                           const P2trVector2 *pt,
                                           P2trTriangle *initial_guess,
                                           gdouble *u,
                                           gdouble *v);

#endif
