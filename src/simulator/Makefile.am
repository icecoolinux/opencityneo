#==============================================================================
# Makefile.am for the libsim.a
# $Id: Makefile.am 427 2009-11-23 22:48:26Z neoneurone $
#==============================================================================
AM_CPPFLAGS = \
	-I@top_srcdir@/src \
	-I@top_srcdir@/src/enum \
	-I@top_srcdir@/src/mas \
	-I@top_srcdir@/src/networking \
	-I@top_srcdir@/src/structure

noinst_LIBRARIES = libsim.a

libsim_a_SOURCES = \
	commercialsim.cpp \
	electricitysim.cpp \
	industrialsim.cpp \
	mainsim.cpp \
	residentialsim.cpp \
	simulator.cpp \
	trafficsim.cpp

noinst_HEADERS = \
	commercialsim.h \
	electricitysim.h \
	industrialsim.h \
	mainsim.h \
	residentialsim.h \
	simulator.h \
	trafficsim.h

