project('firmware-sandbox', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

add_project_arguments('-Wno-sign-compare', language: 'cpp')


dep_fmt = dependency('fmt', required: true)
dep_spdlog = dependency('spdlog', required: true)
dep_gtest = dependency('gtest', required: true)

deps = [dep_fmt, dep_spdlog, dep_gtest]
incdir = include_directories('src')

all_tests = ['env', 'stack', 'circular_buffer', 'object_pool', 'doubly_linked_list', 'hash_map']

foreach t : all_tests
  exe = executable('test_' + t, 'tests/test_' + t + '.cpp', include_directories: incdir, dependencies: deps)
  test('Test ' + t.replace('_', ' '), exe)
endforeach
