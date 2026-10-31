libfdisk = dependency('fdisk', required: true)
libudo = dependency('udo', required: true)

libuprov_deps = [libfdisk, libudo]

fs = files([
  'device.c'
])

cmdline_src = files('cmdline.c')
exec_fs = cmdline_src + files('executables/uprov.c')
