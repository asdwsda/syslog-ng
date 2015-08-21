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

#include "java-filter.h"
#include "java-helpers.h"
#include "logmsg.h"

static gboolean
java_filter_eval(FilterExprNode *s, LogMessage **msg, gint num_msg)
{
    JavaFilter *self = (JavaFilter*) s;
    g_assert_cmpint(num_msg, == , 1);

    gboolean result =  java_filter_proxy_eval(self->proxy, msg[0]) ^ s->comp;
    java_machine_detach_thread();

    return result;
}

JNIEXPORT jstring JNICALL
Java_org_syslog_1ng_FilterExprNode_getOption(JNIEnv *env, jobject obj, jlong s, jstring key)
{
    JavaFilter *self = (JavaFilter *)s;
    gchar *value;
    const char *key_str = (*env)->GetStringUTFChars(env, key, NULL);
    if (key_str == NULL)
    {
        return NULL;
    }

    gchar *normalized_key = normalize_key(key_str);
    value = g_hash_table_lookup(self->preferences->options, normalized_key);
    (*env)->ReleaseStringUTFChars(env, key, key_str);
    g_free(normalized_key);

    if (value)
    {
        return (*env)->NewStringUTF(env, value);
    }
    else
    {
      return NULL;
    }
}

void
java_filter_free(FilterExprNode *s)
{
    JavaFilter *self = (JavaFilter*) s;

    if (self->proxy)
        java_filter_proxy_free(self->proxy);

    java_preferences_free(self->preferences);
}

void
java_filter_init(FilterExprNode *s, GlobalConfig *cfg)
{
    JavaFilter *self = (JavaFilter*) s;

    self->proxy = java_filter_proxy_new(self->preferences->class_name, self->preferences->class_path->str, self);

    java_filter_proxy_init(self->proxy);
}

JavaPreferences *
java_filter_get_preferences(FilterExprNode *s)
{
    JavaFilter *self = (JavaFilter*) s;

    return self->preferences;
}

FilterExprNode *
java_filter_new()
{
    JavaFilter *self = g_new0(JavaFilter, 1);
    filter_expr_node_init_instance(&self->super);
    self->super.eval = java_filter_eval;
    self->super.free_fn = java_filter_free;
    self->super.init = java_filter_init;

    self->preferences = java_preferences_new();

    return (FilterExprNode *)self;
}
