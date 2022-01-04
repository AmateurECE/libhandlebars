###############################################################################
# NAME:             meson.build
#
# AUTHOR:           Ethan D. Twardy <ethan.twardy@gmail.com>
#
# DESCRIPTION:      Build script
#
# CREATED:          11/20/2021
#
# LAST EDITED:      01/04/2022
#
# Copyright 2021, Ethan D. Twardy
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
###

project('libhandlebars', 'c', version: '0.2.0')

libhandlebars_sources = files([
  'handlebars/handlebars.c',
  'handlebars/input-context.c',
  'handlebars/string.c',
  'handlebars/vector.c',
  'handlebars/template-context.c',
  'handlebars/nary-tree.c',
  'handlebars/parser.c',
  'handlebars/scanner.c',
  'handlebars/scanner/token-buffer.c',
  'handlebars/scanner/char-stream.c',
])

install_headers(
  'handlebars/handlebars.h',
)

libhandlebars = library(
  'handlebars',
  sources: libhandlebars_sources,
  install: true,
  c_args: ['-Wall', '-Wextra', '-g', '-O0'],
  version: meson.project_version(),
)

pkgconfig = import('pkgconfig')
pkgconfig.generate(libhandlebars, filebase: 'libhandlebars')

unity = dependency('unity', modules: ['unity::framework'])

executable(
  'testhandlebars',
  sources: files([
    'test/main.c',
    'test/test-handlebars.c',
    'test/test-parser.c',
    'test/test-scanner.c',
  ]),
  include_directories: ['handlebars'],
  link_with: [libhandlebars],
  dependencies: [unity],
  c_args: ['-Wall', '-Wextra', '-Os', '-std=c17'],
)

###############################################################################
