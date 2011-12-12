include $(top_srcdir)/Makefile.common

AM_CFLAGS += $(WARN_CFLAGS)
AM_CPPFLAGS += $(WARN_CPPFLAGS)
AM_CXXFLAGS += $(WARN_CXXFLAGS)

INCLUDES = -I$(srcdir)/..

noinst_LIBRARIES = librender.a
noinst_HEADERS = \
	Render.h \
	RenderFrustum.h \
	RenderPostControl.h \
	RenderPostPass.h \
	RenderPostProgram.h \
	RenderResourceManager.h \
	RenderShader.h \
	RenderTexture.h \
	RenderTarget.h

librender_a_SOURCES = \
	Render.cpp \
	RenderFrustum.cpp \
	RenderPostControl.cpp \
	RenderPostPass.cpp \
	RenderPostProgram.cpp \
	RenderResourceManager.cpp \
	RenderShader.cpp \
	RenderTexture.cpp \
	RenderTarget.cpp
	

