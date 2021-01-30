#include <glib-object.h>
#include <glib/gprintf.h>

#define MY_TYPE_GREETER my_greeter_get_type()
G_DECLARE_FINAL_TYPE(MyGreeter, my_greeter, MY, GREETER, GObject)

struct _MyGreeter {
    GObject parent_instance;
    char *name;
};

G_DEFINE_TYPE(MyGreeter, my_greeter, G_TYPE_OBJECT)

void my_greeter_finalize(GObject *obj)
{
    MyGreeter *self = MY_GREETER(obj);
    g_free(self->name);

    G_OBJECT_CLASS(my_greeter_parent_class)->finalize(obj);
}

void my_greeter_init(MyGreeter *self) {}

enum SignalEnums { GREETED = 0, N_SIGNALS };

guint greeter_signals[N_SIGNALS] = {0};

void greeted_handler(MyGreeter *self, void *user_data)
{
    g_printf("Signal 'greeted' called: %s\n", self->name);
}

void my_greeter_class_init(MyGreeterClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = my_greeter_finalize;

    greeter_signals[GREETED] =
        g_signal_newv("greeted", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
                      g_cclosure_new(G_CALLBACK(greeted_handler), NULL, NULL),
                      NULL,        /* accumulator */
                      NULL,        /* accumulator data */
                      NULL,        /* C marshaller */
                      G_TYPE_NONE, /* return type */
                      0,           /* n_params */
                      NULL);       /* param_types */
}

MyGreeter *my_greeter_new(const char *name)
{
    MyGreeter *greeter = g_object_new(MY_TYPE_GREETER, 0);
    greeter->name = g_strdup(name);
    return greeter;
}

void say_hello(MyGreeter *self)
{
    g_printf("Hello, %s\n", self->name);
    g_signal_emit(self, greeter_signals[GREETED], 0);
}

int main(int argc, char *argv[])
{
    MyGreeter *g1 = my_greeter_new("A");
    MyGreeter *g2 = my_greeter_new("B");

    say_hello(g1);
    say_hello(g2);
    g_object_unref(g1);
    g_object_unref(g2);
    return 0;
}
