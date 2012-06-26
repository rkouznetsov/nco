# template type is application
TEMPLATE = app
# name
TARGET = ncwa

CONFIG -= qt
CONFIG += debug_and_release

#nco library
CONFIG( debug, debug|release ) {
    # debug
        win32:LIBS += ../libnco/debug/libnco.lib
        unix:LIBS  += ../libnco/debug/liblibnco.a
} else {
    # release
        win32:LIBS += ../libnco/release/libnco.lib
        unix:LIBS  += ../libnco/release/liblibnco.a
}

#nco avoid writing temporary file
win32:DEFINES += NO_TMP_FL

#netCDF
DEFINES += ENABLE_NETCDF4
DEFINES += HAVE_NETCDF4_H

unix:HEADERS = ../../src/nco/ncap_yacc.h

#ncwa also requires ncap_yacc.y which should already have been built for ncap
#ncwa_SOURCES = ncwa.c ncap_utl.c ncap_lex.l ncap_yacc.c
unix:SOURCES   = ../../src/nco/ncwa.c \
../../src/nco/ncap_utl.c \
../../src/nco/ncap_yacc.c 

unix {
FLEXSOURCES = ../../src/nco/ncap_lex.l
BISONSOURCES = ../../src/nco/ncap_yacc.y	 

flex.commands = flex ${QMAKE_FILE_IN}
flex.input = FLEXSOURCES
flex.output = ../../src/nco/ncap_lex.c
flex.variable_out = SOURCES
flex.name = flex
QMAKE_EXTRA_COMPILERS += flex
	 
bison.commands = bison -d -t -y ${QMAKE_FILE_IN}
bison.input = BISONSOURCES
bison.output = ../../src/nco/ncap_yacc.c
bison.variable_out = SOURCES
bison.name = bison
QMAKE_EXTRA_COMPILERS += bison

bisonheader.commands = @true
bisonheader.input = BISONSOURCES
bisonheader.output = 
bisonheader.variable_out = HEADERS
bisonheader.name = bison header
bisonheader.depends = 
QMAKE_EXTRA_COMPILERS += bisonheader
}



# netCDF library
# gcc settings to use C99
# HDF5 and netCDF LIB order is important
# _BSD_SOURCE and _POSIX_SOURCE needed
unix {
 DEFINES += HAVE_CONFIG_H
 INCLUDEPATH += ../../
 DEFINES += _BSD_SOURCE
 DEFINES += _POSIX_SOURCE
 QMAKE_CFLAGS += -std=c99
 INCLUDEPATH += /usr/local/include
 INCLUDEPATH += /usr/local
 LIBS += /usr/local/lib/libnetcdf.a
 LIBS += /usr/local/lib/libhdf5_hl.a
 LIBS += /usr/local/lib/libhdf5.a
 LIBS += /usr/lib/x86_64-linux-gnu/libz.a
 LIBS += /usr/lib/x86_64-linux-gnu/libudunits2.a
 LIBS += /usr/lib/x86_64-linux-gnu/libexpat.a
 LIBS += /usr/lib/x86_64-linux-gnu/libcurl.a
}
win32 {
 INCLUDEPATH += $(HEADER_NETCDF)
 LIBS += $(LIB_NETCDF)
 LIBS += $(LIB_DISPATCH)
 LIBS += $(LIB_NETCDF4)
 LIBS += $(LIB_HDF5)
 LIBS += $(LIB_HDF5_HL)
 LIBS += $(LIB_ZLIB)
 LIBS += $(LIB_SZIP)
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 CONFIG += console
}
