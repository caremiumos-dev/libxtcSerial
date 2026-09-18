lib_LTLIBRARIES = libxtcSerial.la

libxtcSerial_la_SOURCES = xtcSerial.cpp xtcserial_core.cpp

libxtcSerial_la_CPPFLAGS = -I$(top_srcdir)/include

AM_CXXFLAGS =

libxtcSerial_la_CXXFLAGS = -fvisibility=hidden -fvisibility-inlines-hidden $(XC_CXXFLAGS)

libxtcSerial_la_LDFLAGS = -avoid-version -no-undefined $(XC_LDFLAGS)

libxtcSerial_la_LIBADD = $(XC_LIBS)