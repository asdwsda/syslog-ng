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

#include "java-helpers.h"
#include "java-preferences.h"

gchar *
java_preferences_get_option(JavaPreferences *self, const gchar *key)
{
  gchar *value;
  gchar *normalized_key = g_strdup(key);
  normalized_key = normalize_key(normalized_key);
  value = g_hash_table_lookup(self->options, normalized_key);
  g_free(normalized_key);

  return value;
}

void
java_preferences_set_option(JavaPreferences *self, const gchar* key, const gchar* value)
{
    gchar *normalized_key = normalize_key(key);
    g_hash_table_insert(self->options, normalized_key, g_strdup(value));
}

void
java_preferences_set_class_path(JavaPreferences *self, const gchar *class_path)
{
  g_string_assign(self->class_path, class_path);
}

void
java_preferences_set_class_name(JavaPreferences *self, const gchar *class_name)
{
  g_free(self->class_name);
  self->class_name = g_strdup(class_name);
}

void
java_preferences_free(JavaPreferences *self)
{
    g_free(self->class_name);
    g_string_free(self->class_path, TRUE);
    g_hash_table_unref(self->options);
}

JavaPreferences *
java_preferences_new()
{
    JavaPreferences *self = g_new0(JavaPreferences, 1);
    self->class_path = g_string_new(".");
    self->options = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    return self;
}
