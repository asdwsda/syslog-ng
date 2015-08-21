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
#include "proxies/java-tf-proxy.h"

#include <string.h>
#include <ctype.h>

typedef struct _TFJavaState
{
  TFSimpleFuncState super;
  JavaPreferences *preferences;
  JavaTFProxy *proxy;
} TFJavaState;

static gboolean
__set_class_path(const gchar *option_name, const gchar *value, gpointer data, GError **error)
{
  JavaPreferences *preferences = (JavaPreferences *) data;
  if (!value)
    {
      g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
       "Error setting class path: value cannot be empty");
      return FALSE;
    }
  java_preferences_set_class_path(preferences, value);
  return TRUE;
}

static gboolean
__set_class_name(const gchar *option_name, const gchar *value, gpointer data, GError **error)
{
  JavaPreferences *preferences = (JavaPreferences *) data;
  if (!value)
    {
      g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
       "Error setting class name: value cannot be empty");
      return FALSE;
    }
  java_preferences_set_class_name(preferences, value);
  return TRUE;
}

gboolean
__set_option(const gchar *option_name, const gchar *value, gpointer data, GError **error)
{
  JavaPreferences *preferences = (JavaPreferences *) data;
  gchar **option;

  if (strchr(value, '=') == NULL)
    {
      g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
       "Error parsing option: expected an equal sign in name=value pair");
      return FALSE;
    }

  option = g_strsplit(value, "=", 2);

  java_preferences_set_option(preferences, option[0], option[1]);

  g_strfreev(option);

  return TRUE;
}

static gboolean
__parse_cmd_line_preferences(LogTemplateFunction *self, gpointer s, LogTemplate *parent, gint argc, gchar *argv[], GError **error)
{
  TFJavaState *state = (TFJavaState *) s;

  GOptionContext *ctx;
  GOptionEntry java_tf_options[] = {
    { "class_name", 'n', 0, G_OPTION_ARG_CALLBACK, __set_class_name, NULL, NULL },
    { "class_path", 'p', 0, G_OPTION_ARG_CALLBACK, __set_class_path, NULL, NULL },
    { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_CALLBACK, __set_option, NULL, NULL},
    { NULL }
  };

  gpointer *user_data = (gpointer) state->preferences;
  ctx = g_option_context_new("java-tf");

  GOptionGroup *group;
  group = g_option_group_new(NULL, NULL, NULL, user_data, NULL);
  g_option_group_add_entries(group, java_tf_options);
  g_option_context_set_main_group(ctx, group);

  if (!g_option_context_parse(ctx, &argc, &argv, error))
    {
      g_option_context_free(ctx);
      return FALSE;
    }
  if (!g_option_context_parse(ctx, &argc, &argv, error))
  g_option_context_free(ctx);

  if (!tf_simple_func_prepare(self, state, parent, argc, argv, error))
    {
      g_free(state);
      return FALSE;
    }

  return TRUE;
}

static gboolean
java_tf_prepare(LogTemplateFunction *self, gpointer s, LogTemplate *parent, gint argc, gchar *argv[], GError **error)
{
  TFJavaState *state = (TFJavaState *) s;
  gboolean result;

  state->preferences = java_preferences_new();
  result =  __parse_cmd_line_preferences(self, s, parent, argc, argv, error);
  state->proxy = java_tf_proxy_new(state->preferences->class_name, state->preferences->class_path->str, self);

  return result;
}

static void
java_tf_call(LogTemplateFunction *self, gpointer s, const LogTemplateInvokeArgs *args, GString *result)
{
  TFJavaState *state = (TFJavaState *) s;
  gchar *res;

  LogMessage *msg = args->messages[0];

  res = java_tf_proxy_call(state->proxy, msg);

  g_string_append(result, res);
}

static void
java_tf_free_state(gpointer s)
{
  TFJavaState *state = (TFJavaState *) s;

  if (state->proxy)
    {
      java_tf_proxy_free(state->proxy);
    }

  if (state->preferences)
    {
      java_preferences_free(state->preferences);
    }

  tf_simple_func_free_state(&state->super);
}

TEMPLATE_FUNCTION(TFJavaState, java_tf, java_tf_prepare, NULL, java_tf_call, java_tf_free_state, NULL);
