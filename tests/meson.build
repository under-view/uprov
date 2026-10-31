libcmocka = dependency('cmocka', required: true)
block_device = get_option('block-device')

progs = [
  'test-device.c',
  'test-version.c',
  'test-cmdline.c',
]

original_args = pargs
foreach p : progs
  exec_name = p.substring(0,-2) # remove .c extension from name

  test_exec_fs = files(p)

  if p == 'test-device.c'
    pargs += [
      '-DBLOCK_DEVICE="' + block_device + '"',
    ]
  endif

  if p == 'test-cmdline.c'
    test_exec_fs += cmdline_src
  endif

  exec = executable(exec_name,
                    test_exec_fs,
                    link_with: libuprov,
                    dependencies: [libcmocka, libudo],
                    include_directories: [inc],
                    c_args: pargs,
                    install: false)

  test(exec_name, exec)
  pargs = original_args
endforeach
