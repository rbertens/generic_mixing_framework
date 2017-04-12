# makefile for ROOT only part of the generic mixing framework
# needs to know where your root and fastjet installations can be found

FASTJETCONFIG 	:= /home/rbertens/Documents/CERN/alice/JET_SOFTWARE/fastjet-3.0.6_BUILD/bin/fastjet-config
CXX           	= g++
F77		= gfortran
INCLUDES 	= -I/usr/include -I./filter -I./reader -I./mixer
CXXFLAGS      =   -O -D_GNU_SOURCE -O2 -g $(INCLUDES)
FLAGS 	+= -fno-second-underscore

UNAME = $(shell uname)

FASTJETLIBS := $(shell $(FASTJETCONFIG) --libs)
FASTJETFLAGS := $(shell $(FASTJETCONFIG) --cxxflags)

ROOTLIBS := $(shell root-config  --libs)
ROOTFLAGS := $(shell root-config --cflags)

CXXFLAGS += $(FASTJETFLAGS) $(ROOTFLAGS)
FLAGS += $(FASTJETLIBS) $(ROOTLIBS)

PLATFORM=$(shell uname)
LINK_LIBS     += -lgfortran

################################################################################ definition of the compiler options
#	-I location of directory containing include files
#	-L location of directory containing libraries
#       -lname include the library from -L location called libname.a
#	   -lg2c is the library containing info on converting fortran to C
#          -lf   is the library containing the intrinsic for HPUX only.
#	-shared make a shared library as output
#	-fPIC produce position independent code
#        necessary on some platforms (including HPUX) for -shared
# 	-fpic ^^ same(?)
#	-O optimizes
#	-g produces output for the debugger
#       -pg produces output for gprof profiler
#       note: if you want to see all warnings and ensure ansi standard 
#             compatibility, use:
#             -pipe -ansi -pedantic -fnonnull-objects \
#             -W -Wall -Wwrite-strings -Wpointer-arith -Wnested-externs \
#             -Woverloaded-virtual -Wbad-function-cast -fnonnull-objects
#       The proper order for cernlib libraries is:
#       -lpawlib -lgraflib -lgrafX11 -lmathlib -lkernlib -lpacklib -ljetset74
#
# makefile syntax:
#        for target thedir/target.suf from source anotherdir/source.suf2
#        ${*D}  = thedir
#        ${*F}  = target
#        $*     = thedir/target
#        $@     = thedir/target.suf
#        $<     = anotherdir/source.suf2
#  

###############################################################################
#
.SUFFIXES:      .o .cxx
all:	generic_mixing_framework

generic_mixing_framework: generic_mixing_framework.o
	@echo "Building $@ ..."
	$(CXX) $(FLAGS) generic_mixing_framework.o \
	        $(LINK_LIBS) -o $@

analyze_hepmc_jet_shapes_constsub: analyze_hepmc_jet_shapes_constsub.o ExampleShapes.o
	@echo "Building $@ ..."
	$(CXX) $(FLAGS) $^ \
		$(HepMClib) \
	        $(LINK_LIBS) -lfastjetcontribfragile -o $@

###############################################################################
# instructions for building a .o file from a .cc file
#
.cxx.o:         $(HDRS) $<
	@echo "Compiling $< with $(CXX) ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

###############################################################################
# gmake clean       removes all garbage from HepMC directories.
#
clean:
	rm -f *.o

###############################################################################
# gmake distclean       removes all compiled libraries, executables, +garbage
#                       to prepare the package for distribution
distclean: 
	$(MAKE) clean --no-print-directory
	rm -f *.exe
	rm -f *.dat

