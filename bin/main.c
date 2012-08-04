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

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include <poly2tri-c/p2t/poly2tri.h>

#include <poly2tri-c/refine/refine.h>
#include <poly2tri-c/render/svg-plot.h>
#include <poly2tri-c/render/mesh-render.h>

#include <string.h>


static gint refine_max_steps = 1000;
static gboolean debug_print = TRUE;
static gboolean verbose = TRUE;
static gchar *input_file = NULL;
static gchar *output_file = NULL;
static gboolean render_mesh = FALSE;
static gboolean render_svg = FALSE;
static gint mesh_width = 100;
static gint mesh_height = 100;

static gfloat min_x = + G_MAXFLOAT;
static gfloat min_y = + G_MAXFLOAT;
static gfloat max_x = - G_MAXFLOAT;
static gfloat max_y = - G_MAXFLOAT;

static GOptionEntry entries[] =
{
  { "refine-max-steps", 'r', 0, G_OPTION_ARG_INT,      &refine_max_steps, "Set maximal refinement steps to N", "N" },
  { "verbose",          'v', 0, G_OPTION_ARG_NONE,     &verbose,          "Print output?",                     NULL },
  { "debug",            'd', 0, G_OPTION_ARG_NONE,     &debug_print,      "Enable debug printing",             NULL },
  { "input",            'i', 0, G_OPTION_ARG_FILENAME, &input_file,       "Use input file at FILE_IN",         "FILE_IN" },
  { "output",           'o', 0, G_OPTION_ARG_FILENAME, &output_file,      "Use output file at FILE_OUT",       "FILE_OUT" },
  { "render-mesh",      'm', 0, G_OPTION_ARG_NONE,     &render_mesh,      "Render a color mesh of the result", NULL },
  { "mesh-width",       'w', 0, G_OPTION_ARG_INT,      &mesh_width,       "The width of the color mesh image", NULL },
  { "mesh-height",      'h', 0, G_OPTION_ARG_INT,      &mesh_height,      "The height of the color mesh iamge",NULL },
  { "render-svg",       's', 0, G_OPTION_ARG_NONE,     &render_svg,       "Render an outline of the result",   NULL },
  { NULL }
};

typedef gfloat Color3f[3];
typedef gfloat Point2f[2];

/**
 * read_points_file:
 * @param path The path to the points & colors file
 * @param points An pointer to an array of pointers to @ref P2RrPoint will be returned
 *          here. NULL can be passed.
 * @param colors An pointer to an array of colors will be returned here. NULL can be
 *          passed.
 *
 *
 */
void
read_points_file (const gchar       *path,
                  P2tPointPtrArray  *points,
                  P2tPointPtrArray  *pointsH,
                  P2tPointPtrArray  *pointsS,
                  GArray           **colors)
{
  FILE *f = fopen (path, "r");
  gint countPts = 0, countPtsH = 0, countPtsS = 0, countCls = 0;

  if (f == NULL)
    {
      g_print ("Error! Could not read input file!");
      exit (1);
    }

  if (verbose)
    g_print ("Now parsing \"%s\"\n", path);

  if (points != NULL) *points = g_ptr_array_new ();
  if (pointsH != NULL) *pointsH = g_ptr_array_new ();
  if (pointsS != NULL) *pointsS = g_ptr_array_new ();
  if (colors != NULL) *colors = g_array_new (FALSE, FALSE, sizeof (Color3f));

  if (debug_print && points == NULL) g_print ("Points will not be kept\n");
  if (debug_print && pointsH == NULL) g_print ("Points Hole will not be kept\n");
  if (debug_print && pointsS == NULL) g_print ("A steiner point will not be kept\n");
  if (debug_print && colors == NULL) g_print ("Colors will not be kept\n");

  while (! feof (f))
    {
      Color3f col = { 0, 0, 0 };
      Point2f ptc = { 0, 0 };
      Point2f pth = { 0, 0 };
      Point2f pts = { 0, 0 };
      gboolean foundCol = FALSE, foundPt = FALSE, foundPtH = FALSE, foundPtS = FALSE;

      /* Read only while we have valid points */
      gint readSize = fscanf (f, "@ %f %f ", &ptc[0], &ptc[1]);

      if (readSize > 0)
        {
          if (readSize != 2)
            {
              g_error ("Error! %d is an unexpected number of floats after point '@' declaration!\n", readSize);
              exit (1);
            }

          foundPt = TRUE;

          if (points != NULL)
            {
              g_ptr_array_add (*points, p2t_point_new_dd (ptc[0], ptc[1]));
              min_x = MIN(ptc[0], min_x);
              min_y = MIN(ptc[1], min_y);
              max_x = MAX(ptc[0], max_x);
              max_y = MAX(ptc[1], max_y);
              countPts++;
            }
        }

      readSize = fscanf (f, "& %f %f ", &pth[0], &pth[1]);

      if (readSize > 0)
        {
          if (readSize != 2)
            {
              g_error ("Error! %d is an unexpected number of floats after point '&' declaration!\n", readSize);
              exit (1);
            }

          foundPtH = TRUE;

          if (pointsH != NULL)
            {
              g_ptr_array_add (*pointsH, p2t_point_new_dd (pth[0], pth[1]));
              countPtsH++;
            }
        }

      readSize = fscanf (f, "* %f %f ", &pts[0], &pts[1]);

      if (readSize > 0)
        {
          if (readSize != 2)
            {
              g_error ("Error! %d is an unexpected number of floats after point '*' declaration!\n", readSize);
              exit (1);
            }

          foundPtS = TRUE;

          if (pointsS != NULL)
            {
              g_ptr_array_add (*pointsS, p2t_point_new_dd (pts[0], pts[1]));
              countPtsS++;
            }
        }

     readSize = fscanf (f, "# %f %f %f ", &col[0], &col[1], &col[2]);

     if (readSize > 0)
       {
          if (readSize != 1 && readSize != 3)
            {
              g_error ("Error! %d is an unexpected number of floats after color '#' declaration!\n", readSize);
              exit (1);
            }

          foundCol = TRUE;

          /* Did we read Gray color information? */
          if (readSize == 1)
            col[1] = col[2] = col[0];

          if (colors != NULL)
            {
              g_array_append_val (*colors, ptc);
              countCls++;
            }
       }

     if (!foundCol && !foundPt && !foundPtH && !foundPtS)
       break;
    }

  fclose (f);

  if (verbose)
    g_print ("Read %d points, %d points hole, %d a steiner points and %d colors\n", countPts, countPtsH, countPtsS, countCls);
}

void
free_read_results (P2tPointPtrArray  *points,
                   P2tPointPtrArray  *pointsH,
                   P2tPointPtrArray  *pointsS,
                   GArray           **colors)
{
  gint i;

  if (points != NULL)
    {
      for (i = 0; i < (*points)->len; i++)
        p2t_point_free (point_index (*points, i));
      g_ptr_array_free (*points, TRUE);
    }

  if (pointsH != NULL)
    {
      for (i = 0; i < (*pointsH)->len; i++)
        p2t_point_free (point_index (*pointsH, i));
      g_ptr_array_free (*pointsH, TRUE);
    }

  if (pointsS != NULL)
    {
      for (i = 0; i < (*pointsS)->len; i++)
        p2t_point_free (point_index (*pointsS, i));
      g_ptr_array_free (*pointsS, TRUE);
    }

  if (colors != NULL)
    g_array_free (*colors, TRUE);
}

/* Calculate a "deterministic random" color for each point
 * based on its memory address. Since we know that least-significant bytes
 * of the point address will change more than the mor-important ones, we
 * make sure to take them into consideration in all the color channels.
 */
static void
test_point_to_color (P2trPoint* point, guint8 *dest, gpointer user_data)
{
  gulong value = (gulong) point;
  guchar b1 = value & 0xff, b2 = (value & 0xff00) >> 2, b3 = (value & 0xff0000) >> 4;
  dest[0] = b1;
  dest[1] = (b1 ^ b2);
  dest[2] = (b1 ^ b3);
}

void
p2tr_write_rgb_ppm (FILE            *f,
                    guint8          *dest,
                    P2trImageConfig *config)
{
  gint x, y;
  guint8 *pixel;

  fprintf (f, "P3\n");
  fprintf (f, "%d %d\n", config->x_samples, config->y_samples);
  fprintf (f, "255\n");

  pixel = dest;

  for (y = 0; y < config->y_samples; y++)
    {
      for (x = 0; x < config->x_samples; x++)
        {
          if (pixel[3] <= 0.5)
            fprintf (f, "  0   0   0");
          else
            fprintf (f, "%3d %3d %3d", pixel[0], pixel[1], pixel[2]);

          if (x != config->x_samples - 1)
            fprintf (f, "   ");

          pixel += 4;
        }
      fprintf (f, "\n");
    }
}

gint main (int argc, char *argv[])
{
  FILE *svg_out = NULL, *mesh_out = NULL;
  gchar *svg_out_path, *mesh_out_path;

  GError *error = NULL;
  GOptionContext *context;

  GPtrArray *pts;
  GPtrArray *ptsH;
  GPtrArray *ptsS;
  GArray    *colors;

  P2tCDT *cdt;
  P2trCDT *rcdt;
  P2trRefiner *refiner;

  context = g_option_context_new ("- Create a fine mesh from a given PSLG");
  g_option_context_add_main_entries (context, entries, NULL);

  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
      g_print ("option parsing failed: %s\n", error->message);
      exit (1);
    }

  g_option_context_free (context);

  if (input_file == NULL)
    {
      g_print ("No input file given. Stop.");
      exit (1);
    }

  if (! g_file_test (input_file, G_FILE_TEST_EXISTS))
    {
      g_print ("Input file does not exist. Stop.");
      exit (1);
    }

  if (output_file == NULL && (render_svg || render_mesh))
    {
      g_print ("No output file given. Stop.");
      exit (1);
    }

  if (render_svg)
    {
      svg_out_path = g_newa (gchar, strlen (output_file) + 4);
      sprintf (svg_out_path, "%s.svg", output_file);

      if ((svg_out = fopen (svg_out_path, "w")) == NULL)
        {
          g_print ("Can't open the svg output file. Stop.");
          exit (1);
        }
    }

  if (render_mesh)
    {
      mesh_out_path = g_newa (gchar, strlen (output_file) + 4);
      sprintf (mesh_out_path, "%s.ppm", output_file);

      if ((mesh_out = fopen (mesh_out_path, "w")) == NULL)
        {
          g_print ("Can't open the mesh output file. Stop.");
          exit (1);
        }
    }

  read_points_file (input_file, &pts, &ptsH, &ptsS, &colors);

  cdt = p2t_cdt_new (pts);
  if (ptsH->len > 0)
    p2t_cdt_add_hole (cdt, ptsH);
  if (ptsS->len > 0)
    {
    gint i;
    for (i = 0; i < ptsS->len; i++)
        p2t_cdt_add_point (cdt, point_index (ptsS, i));
    }
  p2t_cdt_triangulate (cdt);

  rcdt = p2tr_cdt_new (cdt);
  p2t_cdt_free (cdt);

  if (refine_max_steps > 0)
    {
      g_print ("Refining the mesh!\n");
      refiner = p2tr_refiner_new (G_PI / 6, p2tr_refiner_false_too_big, rcdt);
      p2tr_refiner_refine (refiner, refine_max_steps, NULL);
      p2tr_refiner_free (refiner);
    }

  if (render_svg)
    {
      g_print ("Rendering SVG outline!");
      p2tr_render_svg (rcdt->mesh, svg_out);
      fclose (svg_out);
    }

  if (render_mesh)
    {
      P2trImageConfig imc;
      guint8 *im;

      g_print ("Rendering color interpolation!");

      imc.cpp = 3;
      imc.min_x = min_x;
      imc.min_y = min_y;
      imc.step_x = (max_x - min_x) / ((gfloat) mesh_width - 1);
      imc.step_y = (max_y - min_y) / ((gfloat) mesh_height - 1);
      imc.x_samples = mesh_width;
      imc.y_samples = mesh_height;
      imc.alpha_last = TRUE;

      im = g_new (guint8, (1 + imc.cpp) * imc.x_samples * imc.y_samples);

      p2tr_mesh_render_b (rcdt->mesh, im, &imc, test_point_to_color, NULL);

      p2tr_write_rgb_ppm (mesh_out, im, &imc);
      fclose (mesh_out);

      g_free (im);
    }

  p2tr_cdt_free (rcdt);
  free_read_results (&pts, &ptsH, &ptsS, &colors);

  return 0;
}
