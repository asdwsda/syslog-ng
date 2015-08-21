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

#ifndef JAVA_FILTER_H_INCLUDED
#define JAVA_FILTER_H_INCLUDED

#include "filter/filter-expr.h"
#include "proxies/java-filter-proxy.h"

typedef struct
{
    FilterExprNode super;
    JavaFilterProxy *proxy;
    GString *class_path;
    gchar *class_name;
    GHashTable *options;
} JavaFilter;

FilterExprNode* java_filter_new();
void java_filter_set_class_path(FilterExprNode *s, const gchar *class_path);
void java_filter_set_class_name(FilterExprNode *s, const gchar *class_name);
void java_filter_set_option(FilterExprNode *s, const gchar* key, const gchar* value);

#endif