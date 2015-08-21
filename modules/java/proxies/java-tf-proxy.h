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

#ifndef JAVA_TF_PROXY_H_
#define JAVA_TF_PROXY_H_

#include <jni.h>
#include <syslog-ng.h>
#include "java_machine.h"

typedef struct _JavaTFProxy JavaTFProxy;

JavaTFProxy* java_tf_proxy_new(const gchar *class_name, const gchar *class_path, gpointer handle);

gchar* java_tf_proxy_call(JavaTFProxy *self, LogMessage *msg);

void java_tf_proxy_free(JavaTFProxy *self);

#endif
