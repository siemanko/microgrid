
cdef extern from "/home/sidor/projects/microgrid/interface/text.h":
    cdef int what(int a, int b)

def what_wrapper(a,b):
    return what(a,b)
