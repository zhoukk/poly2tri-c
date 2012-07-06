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

#ifndef __P2TC_REFINE_MESH_ACTION_H__
#define __P2TC_REFINE_MESH_ACTION_H__

#include <glib.h>

typedef enum
{
  P2TR_MESH_ACTION_POINT,
  P2TR_MESH_ACTION_EDGE,
  P2TR_MESH_ACTION_TRIANGLE
} P2trMeshActionType;

typedef struct P2trMeshActionPoint_
{
  P2trPoint *point;
} P2trMeshActionPoint;

typedef struct P2trMeshActionEdge_
{
  P2trVEdge *vedge;
  gboolean   constrained;
} P2trMeshActionEdge;

typedef struct P2trMeshActionTriangle_
{
  P2trVTriangle *vtri;
} P2trMeshActionTriangle;

typedef struct P2trMeshAction_
{
  P2trMeshActionType  type;
  gboolean            added;
  gint                refcount;
  union {
    P2trMeshActionPoint    action_point;
    P2trMeshActionEdge     action_edge;
    P2trMeshActionTriangle action_tri;
  }                   action;
} P2trMeshAction;

P2trMeshAction*  p2tr_mesh_action_new_point      (P2trPoint      *point);
P2trMeshAction*  p2tr_mesh_action_del_point      (P2trPoint      *point);
P2trMeshAction*  p2tr_mesh_action_new_edge       (P2trEdge       *edge);
P2trMeshAction*  p2tr_mesh_action_del_edge       (P2trEdge       *edge);
P2trMeshAction*  p2tr_mesh_action_new_triangle   (P2trTriangle   *tri);
P2trMeshAction*  p2tr_mesh_action_del_triangle   (P2trTriangle   *tri);

P2trMeshAction*  p2tr_mesh_action_ref            (P2trMeshAction *self);
void             p2tr_mesh_action_unref          (P2trMeshAction *self);
void             p2tr_mesh_action_free           (P2trMeshAction *self);
void             p2tr_mesh_action_undo           (P2trMeshAction *self,
                                                  P2trMesh       *mesh);

#endif
