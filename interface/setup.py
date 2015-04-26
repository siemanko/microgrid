import fnmatch

from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
from Cython.Distutils import build_ext
from os import walk, utime
from os.path import join, dirname, realpath

def find_files(path, pattern):
    for root, dirnames, filenames in walk(path):
        for filename in fnmatch.filter(filenames, pattern):
            yield join(root, filename)

cmdclass = {}
ext_modules = []

SCRIPT_DIR = dirname(realpath(__file__))
CPATH = join(SCRIPT_DIR, 'cython')

# touch all pyx file to force recompile
for cython_file in find_files(CPATH, '*.pyx'):
    # touch
    utime(cython_file, None)

ext_modules.append(Extension(
    "bindings.communication",
    [
        join(CPATH, "communication.pyx"),
        join(CPATH, "shared/communication/ethermini.c"),
        join(CPATH, "shared/algorithm/circular_buffer.c"),
    ],
    include_dirs = [CPATH],
))
build_ext.cython_c_in_temp = True
cmdclass.update({ 'build_ext': build_ext })

setup(
    name='microgrid_buddy',
    cmdclass=cmdclass,
    ext_modules=ext_modules,
)
