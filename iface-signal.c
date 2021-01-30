#include <glib-object.h>
#include <glib/gprintf.h>

/* Header File Declarations: */

#define BASE_TYPE_GREETER base_greeter_get_type()
G_DECLARE_INTERFACE(BaseGreeter, base_greeter, BASE, GREETER, GObject)

struct _BaseGreeterInterface {
    GTypeInterface parent_iface;

    void (*greet)(BaseGreeter *self);
    void (*handle_greeted)(BaseGreeter *self, void *user_data);
};

void base_greeter_greet(BaseGreeter *greeter);

#define MY_TYPE_GREETER my_greeter_get_type()
G_DECLARE_FINAL_TYPE(MyGreeter, my_greeter, MY, GREETER, GObject)

/* BaseGreeter Definititions */

G_DEFINE_INTERFACE(BaseGreeter, base_greeter, G_TYPE_OBJECT)

enum SignalEnums { GREETED = 0, N_SIGNALS };

guint greeter_signals[N_SIGNALS] = {0};

void base_greeter_default_init(BaseGreeterInterface *iface)
{
    greeter_signals[GREETED] =
        g_signal_new("greeted", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(BaseGreeterInterface, handle_greeted),
                     NULL,        /* accumulator */
                     NULL,        /* accumulator data */
                     NULL,        /* C marshaller */
                     G_TYPE_NONE, /* return type */
                     0,           /* n_params */
                     NULL);       /* param_types */
}

void base_greeter_greet(BaseGreeter *self)
{
    BaseGreeterInterface *iface = BASE_GREETER_GET_IFACE(self);
    iface->greet(self);
    g_signal_emit(self, greeter_signals[GREETED], 0);
}

/* MyGreeter Definitions */

struct _MyGreeter {
    GObject parent_instance;
    char *name;
};

void my_greeter_interface_init(BaseGreeterInterface *iface);

G_DEFINE_TYPE_WITH_CODE(MyGreeter, my_greeter, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(BASE_TYPE_GREETER,
                                              my_greeter_interface_init))

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

void my_greeter_interface_init(BaseGreeterInterface *iface)
{
    iface->greet = my_greeter_greet;
    iface->handle_greeted = my_greeter_handle_greeted;
}

void my_greeter_class_init(MyGreeterClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = my_greeter_finalize;
}

MyGreeter *my_greeter_new(const char *name)
{
    MyGreeter *greeter = g_object_new(MY_TYPE_GREETER, 0);
    greeter->name = g_strdup(name);
    return greeter;
}

int main(int argc, char *argv[])
{
    MyGreeter *g1 = my_greeter_new("A");
    MyGreeter *g2 = my_greeter_new("B");

    base_greeter_greet(BASE_GREETER(g1));
    base_greeter_greet(BASE_GREETER(g2));

    g_object_unref(g1);
    g_object_unref(g2);
    return 0;
}
