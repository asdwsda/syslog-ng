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

#include "java-parser.h"
#include "java-helpers.h"

JNIEXPORT jstring JNICALL
Java_org_syslog_1ng_LogParser_getOption(JNIEnv *env, jobject obj, jlong s, jstring key)
{
    JavaParser *self = (JavaParser *)s;
    gchar *value;
    const char *key_str = (*env)->GetStringUTFChars(env, key, NULL);
    if (key_str == NULL)
      {
        return NULL;
      }
    value = java_preferences_get_option(self->preferences, key_str);
    (*env)->ReleaseStringUTFChars(env, key, key_str);  // release resources

    if (value)
      {
        return (*env)->NewStringUTF(env, value);
      }
    else
      {
        return NULL;
      }
}

static gboolean
java_parser_init(LogPipe *parser)
{
  JavaParser *self = (JavaParser *)parser;

  self->proxy = java_parser_proxy_new(self->preferences->class_name, self->preferences->class_path->str, self);

  java_parser_proxy_init(self->proxy);
  return TRUE;
};

static gboolean
java_parser_process(LogParser *s, LogMessage **pmsg, const LogPathOptions *path_options, const gchar *input, gsize input_len)
{
  JavaParser *self = (JavaParser *)s;
  LogMessage *msg = log_msg_make_writable(pmsg, path_options);

  gboolean result = java_parser_proxy_process(self->proxy, msg, input, input_len);
  java_machine_detach_thread();

  return result;
};

static LogPipe *
java_parser_clone(LogPipe *s)
{
  JavaParser *self = (JavaParser *) s;

  JavaParser *cloned = (JavaParser *) java_parser_new(log_pipe_get_config(&self->super.super));

  cloned->super.template = log_template_ref(self->super.template);
  clone_java_preferences(self->preferences, cloned->preferences);

  return &cloned->super.super;
};

static void
java_parser_free(LogPipe *s)
{
  JavaParser *self = (JavaParser *)s;

  if (self->proxy)
    java_parser_proxy_free(self->proxy);

  log_parser_free_method(s);
  java_preferences_free(self->preferences);
};

JavaPreferences *
java_parser_get_preferences(LogParser *s)
{
  JavaParser *self = (JavaParser *) s;

  return self->preferences;
}

LogParser *
java_parser_new(GlobalConfig *cfg)
{
  JavaParser *self = g_new0(JavaParser, 1);
  log_parser_init_instance(&self->super, cfg);
  self->super.super.init = java_parser_init;
  self->super.process = java_parser_process;
  self->super.super.clone = java_parser_clone;
  self->super.super.free_fn = java_parser_free;

  self->preferences = java_preferences_new();

  return &self->super;
};
