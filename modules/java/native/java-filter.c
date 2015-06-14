#include "java-filter.h"
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


// TODO: ez duplikatum
static gchar *
__normalize_key(const gchar *buffer)
{
    const gchar from = '-';
    const gchar to = '_';
    gchar *p;
    gchar *normalized_key = g_strdup(buffer);
    p = normalized_key;
    while (*p)
    {
        if (*p == from)
            *p = to;
        p++;
    }
    return normalized_key;
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

    gchar *normalized_key = __normalize_key(key_str);
    value = g_hash_table_lookup(self->options, normalized_key);
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
java_filter_set_option(FilterExprNode *s, gchar* key, gchar* value)
{
    JavaFilter *self = (JavaFilter*) s;
    gchar *normalized_key = __normalize_key(key);
    g_hash_table_insert(self->options, normalized_key, g_strdup(value));
}

void
java_filter_set_class_path(FilterExprNode *s, const gchar *class_path)
{
  JavaFilter *self = (JavaFilter *)s;
  g_string_assign(self->class_path, class_path);
}

void
java_filter_set_class_name(FilterExprNode *s, const gchar *class_name)
{
  JavaFilter *self = (JavaFilter *)s;
  g_free(self->class_name);
  self->class_name = g_strdup(class_name);
}

void
java_filter_free(FilterExprNode *s)
{
    JavaFilter *self = (JavaFilter*) s;

    if (self->proxy)
        java_filter_proxy_free(self->proxy);

    g_free(self->class_name);
    g_string_free(self->class_path, TRUE);
}

void
java_filter_init(FilterExprNode *s, GlobalConfig *cfg)
{
    JavaFilter *self = (JavaFilter*) s;


    msg_debug("Init java filter", evt_tag_str("class_path", self->class_path->str), NULL);
    msg_debug("Init java filter", evt_tag_str("class_name", self->class_name), NULL);
    self->proxy = java_filter_proxy_new(self->class_name, self->class_path->str, self);

    // TODO: kivezetni a cfg-t javaba
    java_filter_proxy_init(self->proxy);
}

FilterExprNode *
java_filter_new()
{
    JavaFilter *self = g_new0(JavaFilter, 1);
    filter_expr_node_init_instance(&self->super);
    self->super.eval = java_filter_eval;
    self->super.free_fn = java_filter_free;
    self->super.init = java_filter_init;

    self->class_path = g_string_new(".");
    self->options = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    return (FilterExprNode *)self;
}