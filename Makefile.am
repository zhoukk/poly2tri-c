SUBDIRS = p2t refine render bin

ACLOCAL_AMFLAGS = -I m4

AM_LDFLAGS = -version-info $(P2TC_LIBRARY_VERSION)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = poly2tri-c.pc

lib_LTLIBRARIES = libpoly2tri-c-@P2TC_API_VERSION@.la

libpoly2tri_c_@P2TC_API_VERSION@_la_LIBADD = p2t/libp2tc.la refine/libp2tc-refine.la render/libp2tc-render.la
libpoly2tri_c_@P2TC_API_VERSION@_la_SOURCES =

