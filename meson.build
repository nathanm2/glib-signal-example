project('glib-signal-example', 'c')

gobject = dependency('gobject-2.0')

executable('class-signal', 'class-signal.c', dependencies: gobject)
executable('class-signal-vtable', 'class-signal-vtable.c',
  dependencies: gobject)
executable('iface-signal', 'iface-signal.c', dependencies: gobject)
