/*
 * Copyright (c) 2015 BalaBit IT Ltd, Budapest, Hungary
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */

#include "java-preferences.h"
#include "template/simple-function.h"
#include "java-tf.h"

#include <string.h>
#include <ctype.h>

typedef struct _TFJavaState
{
  TFSimpleFuncState super;
  JavaPreferences *preferences;
} TFJavaState;

static gboolean
java_tf_prepare(LogTemplateFunction *self, gpointer s, LogTemplate *parent, gint argc, gchar *argv[], GError **error)
{
  TFJavaState *state = (TFJavaState *) s;
  gchar *class_name = "";
  gchar *class_path = "";
  GOptionContext *ctx;
  GOptionEntry java_tf_options[] = {
    { "class-name", 'n', 0, G_OPTION_ARG_STRING, &class_name, NULL, NULL },
    { "class-path", 'p', 0, G_OPTION_ARG_STRING, &class_path, NULL, NULL },
    { NULL }
  };

  ctx = g_option_context_new("java");
  g_option_context_add_main_entries(ctx, java_tf_options, NULL);

  if (!g_option_context_parse(ctx, &argc, &argv, error))
    {
      g_option_context_free(ctx);
      g_free(argv);
      return FALSE;
    }
  g_option_context_free(ctx);

  if (!tf_simple_func_prepare(self, state, parent, argc, argv, error))
    {
      g_free(state);
      return FALSE;
    }
  java_preferences_set_class_name(state->preferences, class_name);
  java_preferences_set_class_path(state->preferences, class_path);
  return TRUE;
}

static void
java_tf_call(LogTemplateFunction *self, gpointer s, const LogTemplateInvokeArgs *args, GString *result)
{
  //TFJavaState *state = (TFJavaState *) s;
  //GString **argv;
  //gint argc;
  //gint i, pos;

  //argv = (GString **) args->bufs->pdata;
  //argc = args->bufs->len;
  g_string_append(result, "DUMMY");
}

static void
java_tf_free_state(gpointer s)
{
  TFJavaState *state = (TFJavaState *) s;

  java_preferences_free(state->preferences);
  tf_simple_func_free_state(&state->super);
}

TEMPLATE_FUNCTION(TFJavaState, java_tf, java_tf_prepare, NULL, java_tf_call, java_tf_free_state, NULL);
