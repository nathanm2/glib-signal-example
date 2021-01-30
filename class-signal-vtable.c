#include <glib-object.h>
#include <glib/gprintf.h>

/* Header File Declarations: */

#define BASE_TYPE_GREETER base_greeter_get_type()
G_DECLARE_DERIVABLE_TYPE(BaseGreeter, base_greeter, BASE, GREETER, GObject)

struct _BaseGreeterClass {
    GObjectClass parent_class;

    void (*greet)(BaseGreeter *self);
    void (*handle_greeted)(BaseGreeter *self, void *user_data);
};

void base_greeter_greet(BaseGreeter *greeter);

#define MY_TYPE_GREETER my_greeter_get_type()
G_DECLARE_FINAL_TYPE(MyGreeter, my_greeter, MY, GREETER, BaseGreeter)

/* BaseGreeter Definititions */

G_DEFINE_TYPE(BaseGreeter, base_greeter, G_TYPE_OBJECT)

enum SignalEnums { GREETED = 0, N_SIGNALS };

guint greeter_signals[N_SIGNALS] = {0};

void base_greeted_default_greet(BaseGreeter *self)
{
    g_printf("Hi\n");
}

void base_greeted_handle_greeted(BaseGreeter *self, void *user_data)
{
    g_printf("base_greeted_handle_greeted called\n");
}

void base_greeter_class_init(BaseGreeterClass *klass)
{
    klass->greet = base_greeted_default_greet;
    klass->handle_greeted = base_greeted_handle_greeted;

    greeter_signals[GREETED] =
        g_signal_new("greeted", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(BaseGreeterClass, handle_greeted),
                     NULL,        /* accumulator */
                     NULL,        /* accumulator data */
                     NULL,        /* C marshaller */
                     G_TYPE_NONE, /* return type */
                     0,           /* n_params */
                     NULL);       /* param_types */
}

void base_greeter_init(BaseGreeter *self) {}

void base_greeter_greet(BaseGreeter *greeter)
{
    BaseGreeterClass *base_greeter_class = BASE_GREETER_GET_CLASS(greeter);
    base_greeter_class->greet(greeter);
    g_signal_emit(greeter, greeter_signals[GREETED], 0);
}

BaseGreeter *base_greeter_new(void)
{
    return g_object_new(BASE_TYPE_GREETER, 0);
}

/* MyGreeter Definitions */

struct _MyGreeter {
    BaseGreeter parent_instance;
    char *name;
};

G_DEFINE_TYPE(MyGreeter, my_greeter, BASE_TYPE_GREETER)

void my_greeter_finalize(GObject *obj)
{
    MyGreeter *self = MY_GREETER(obj);
    g_free(self->name);

    G_OBJECT_CLASS(my_greeter_parent_class)->finalize(obj);
}

void my_greeter_init(MyGreeter *self) {}

void my_greeter_greet(BaseGreeter *obj)
{
    MyGreeter *self = MY_GREETER(obj);
    g_printf("Hello, %s\n", self->name);
}

void my_greeter_handle_greeted(BaseGreeter *self, void *user_data)
{
    g_printf("my_greeter_handle_greeted called\n");
}

void my_greeter_class_init(MyGreeterClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = my_greeter_finalize;

    BaseGreeterClass *base_greeter_class = BASE_GREETER_CLASS(klass);
    base_greeter_class->greet = my_greeter_greet;
    base_greeter_class->handle_greeted = my_greeter_handle_greeted;
}

MyGreeter *my_greeter_new(const char *name)
{
    MyGreeter *greeter = g_object_new(MY_TYPE_GREETER, 0);
    greeter->name = g_strdup(name);
    return greeter;
}

int main(int argc, char *argv[])
{
    BaseGreeter *base = base_greeter_new();
    MyGreeter *g1 = my_greeter_new("A");
    MyGreeter *g2 = my_greeter_new("B");

    base_greeter_greet(base);
    base_greeter_greet(BASE_GREETER(g1));
    base_greeter_greet(BASE_GREETER(g2));

    g_object_unref(base);
    g_object_unref(g1);
    g_object_unref(g2);
    return 0;
}
