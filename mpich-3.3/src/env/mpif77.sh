#! /bin/sh
#
# (C) 2006 by Argonne National Laboratory.
#     See COPYRIGHT in top-level directory.
#
# mpif77
# Simple script to compile and/or link MPI programs.
# This script knows the default flags and libraries, and can handle
# alternative C compilers and the associated flags and libraries.
# The important terms are:
#    includedir, libdir - Directories containing an *installed* mpich
#    prefix, execprefix - Often used to define includedir and libdir
#    F77                - Fortran 77 compiler
#    WRAPPER_FFLAGS     - Any special flags needed to compile 
#    WRAPPER_LDFLAGS    - Any special flags needed to link
#    WRAPPER_LIBS       - Any special libraries needed in order to link
#    F77_OTHER_LIBS     - Yet more libraries, needed just with F77
#    
#    
# We assume that (a) the C compiler can both compile and link programs
#
# Handling of command-line options:
#   This is a little tricky because some options may contain blanks.
#
# Special issues with shared libraries - todo
#
# --------------------------------------------------------------------------
# Set the default values of all variables.
#
# Directory locations: Fixed for any MPI implementation.
# Set from the directory arguments to configure (e.g., --prefix=/usr/local)
prefix=__PREFIX_TO_BE_FILLED_AT_INSTALL_TIME__
exec_prefix=__EXEC_PREFIX_TO_BE_FILLED_AT_INSTALL_TIME__
sysconfdir=__SYSCONFDIR_TO_BE_FILLED_AT_INSTALL_TIME__
includedir=__INCLUDEDIR_TO_BE_FILLED_AT_INSTALL_TIME__
libdir=__LIBDIR_TO_BE_FILLED_AT_INSTALL_TIME__

# Default settings for compiler, flags, and libraries
# Determined by a combination of environment variables and tests within
# configure (e.g., determining whehter -lsocket is needee)
F77="gfortran"
F77CPP=""
MPICH_VERSION="3.3"

enable_wrapper_rpath="yes"
@f77_shlib_conf@

# Internal variables
# Show is set to echo to cause the compilation command to be echoed instead 
# of executed.
Show=eval
#
# End of initialization of variables
#---------------------------------------------------------------------
# Environment Variables.
# The environment variables MPICH_F77 may be used to override the 
# default choices.
# In addition, if there is a file $sysconfdir/mpif77-$F77name.conf, 
# where F77name is the name of the compiler with all spaces replaced by hyphens
# (e.g., "f77 -64" becomes "f77--64", that file is sources, allowing other
# changes to the compilation environment.  See the variables used by the 
# script (defined above)
# Added MPICH_F77_OLD, MPICH_F77 can be used to prefix F77
# with external utility, e.g. setenv MPICH_F77 'eval linkcache $MPICH_F77_OLD'
if [ -n "$MPICH_F77" ] ; then
    MPICH_F77_OLD="$F77"
    F77="$MPICH_F77"
    F77name=`echo $F77 | sed 's/ /-/g'`
    if [ -s $sysconfdir/mpif77-$F77name.conf ] ; then
        . $sysconfdir/mpif77-$F77name.conf
    fi
fi
# Allow a profiling option to be selected through an environment variable
if [ -n "$MPIF77_PROFILE" ] ; then
    profConf=$MPIF77_PROFILE
fi
#
# ------------------------------------------------------------------------
# Argument processing.
# This is somewhat awkward because of the handling of arguments within
# the shell.  We want to handle arguments that include spaces without 
# loosing the spacing (an alternative would be to use a more powerful
# scripting language that would allow us to retain the array of values, 
# which the basic (rather than enhanced) Bourne shell does not.  
#
# Look through the arguments for arguments that indicate compile only.
# If these are *not* found, add the library options

linking=yes
allargs=""
interlib_deps=yes
static_mpi=no
for arg in "$@" ; do
    # Set addarg to no if this arg should be ignored by the C compiler
    addarg=yes
    qarg=$arg
    case $arg in 
 	# ----------------------------------------------------------------
	# Compiler options that affect whether we are linking or no
    -c|-S|-E|-M|-MM)
    # The compiler links by default
    linking=no
    ;;
	# ----------------------------------------------------------------
	# Options that control how we use mpif77 (e.g., -show, 
	# -f77=* -config=*
    -static)
    interlib_deps=no
    ;;
    -static-mpi)
    interlib_deps=no
    static_mpi=yes
    addarg=no
    ;;
    -echo)
    addarg=no
    set -x
    ;;

    -f77=*)
    F77=`echo A$arg | sed -e 's/A-f77=//g'`
    addarg=no
    ;;
    -fc=*)
    F77=`echo A$arg | sed -e 's/A-fc=//g'`
    addarg=no
    ;;

    -show)
    addarg=no
    Show=echo
    ;;
    -config=*)
    addarg=no
    F77name=`echo A$arg | sed -e 's/A-config=//g'`
    if [ -s "$sysconfdir/mpif77-$F77name.conf" ] ; then
        . "$sysconfdir/mpif77-$F77name.conf"
    else
	echo "Configuration file mpif77-$F77name.conf not found"
    fi
    ;;
    -compile-info|-compile_info)
    # -compile_info included for backward compatibility
    Show=echo
    addarg=no
    ;;
    -link-info|-link_info)
    # -link_info included for backward compatibility
    Show=echo
    addarg=no
    ;;
    -v)
    # Pass this argument to the compiler as well.
    echo "mpif77 for MPICH version $MPICH_VERSION"
    # if there is only 1 argument, it must be -v.
    if [ "$#" -eq "1" ] ; then
        linking=no
    fi
    ;;
    -profile=*)
    # Pass the name of a profiling configuration.  As
    # a special case, lib<name>.so or lib<name>.la may be used
    # if the library is in $libdir
    profConf=`echo A$arg | sed -e 's/A-profile=//g'`
    addarg=no
    # Loading the profConf file is handled below
    ;;
    -nativelinking)
    # Internal option to use native compiler for linking without MPI libraries
    nativelinking=yes
    addarg=no
    ;;
    -help)
    NC=`echo "$F77" | sed 's%\/% %g' | awk '{print $NF}' -`
    if [ -f "$sysconfdir/mpixxx_opts.conf" ] ; then
        . $sysconfdir/mpixxx_opts.conf
        echo "    -f77=xxx      - Reset the native compiler to xxx."
        echo "    -fc=xxx       - Reset the native compiler to xxx."
    else
        if [ -f "./mpixxx_opts.conf" ] ; then
            . ./mpixxx_opts.conf
            echo "    -f77=xxx      - Reset the native compiler to xxx."
            echo "    -fc=xxx       - Reset the native compiler to xxx."
        fi
    fi
    exit 0
    ;;
        # -----------------------------------------------------------------
	# Other arguments.  We are careful to handle arguments with 
	# quotes (we try to quote all arguments in case they include 
	# any spaces)
    *\"*) 
    qarg="'"$arg"'"
    case $arg in 
       -D*)
       cppflags="$cppflags $qarg"
       ;;
    esac
    ;;
    *\'*) 
    qarg='\"'"$arg"'\"'
    case $arg in 
       -D*)
       cppflags="$cppflags $qarg"
       ;;
    esac
    ;;

    # The following are special args used to handle .F files when the
    # Fortran compiler itself does not handle these options
    -I*)
    cppflags="$cppflags $arg"
    ;;
    -D*)
    cppflags="$cppflags $arg"
    ;;
    *.F|*.F90|*.fpp|*.FPP)
# If F77CPP is not empty, then we need to do the following:
#    If any input files have the .F or .F90 extension, then    
#        If F77CPP = false, then
#            generate an error message and exit
#        Use F77CPP to convert the file from .F to .f, using 
#            $TMPDIR/f$$-$count.f as the output file name
#            Replace the input file with this name in the args
# This is needed only for very broken systems
#     
    if [ -n "$F77CPP" ] ; then
        if [ "$F77CPP" = "false" ] ; then
            echo "This Fortran compiler does not accept .F or .F90 files"
	    exit 1
        fi
        addarg=no
	# Remove and directory names and extension
	$ext=`expr "$arg" : '.*\(\..*\)'`
        bfile=`basename $arg $ext`
	#
	TMPDIR=${TMPDIR:-/tmp}
        tmpfile=$TMPDIR/f$$-$bfile.f
        if $F77CPP $cppflags $arg > $tmpfile ; then
            # Add this file to the commandline list
	    count=`expr $count + 1`
	    allargs="$allargs $tmpfile"
	    rmfiles="$rmfiles $tmpfile"
        else
	    echo "Aborting compilation because of failure in preprocessing step"
	    echo "for file $arg ."
	    exit 1
        fi
    fi
    # Otherwise, just accept the argument
    ;;
    # - end of special handling for .F files
    
    *)
    qarg="'$arg'"
    ;;

    esac
    if [ $addarg = yes ] ; then
        allargs="$allargs $qarg"
    fi
done

if [ $# -eq 0 ] ; then
    echo "Error: Command line argument is needed!"
    "$0" -help
    exit 1
fi

# -----------------------------------------------------------------------
# Derived variables.  These are assembled from variables set from the
# default, environment, configuration file (if any) and command-line
# options (if any)
f77libs=
if [ "mpifort" != "mpi" ] ; then
    f77libs="-lmpifort"
fi

PROFILE_FOO=
# Handle the case of a profile switch
if [ -n "$profConf" ] ; then
    profConffile=
    if [ -s "$libdir/lib$profConf.a" -o -s "$libdir/lib$profConf.so" ] ; then
	PROFILE_FOO="-l$profConf"
    elif [ -s "$sysconfdir/$profConf.conf" ] ; then
	profConffile="$sysconfdir/$profConf.conf"
    elif [ -s "$profConf.conf" ] ; then
        profConffile="$profConf.conf"
    else
        echo "Profiling configuration file $profConf.conf not found in $sysconfdir"
    fi
    if [ -n "$profConffile" -a -s "$profConffile" ] ; then
	. $profConffile
    fi
fi

final_fflags=" "
final_ldflags=" "
final_libs=""
if test "yes" = "no" -o "${interlib_deps}" = "no" ; then
    final_ldflags="${final_ldflags} "
    final_libs="${final_libs}  -lm  -ludev -lpciaccess  -lxml2   -lunwind -lpthread  -lrt "
fi

# A temporary statement to invoke the compiler
# Place the -L before any args incase there are any mpi libraries in there.
# Eventually, we'll want to move this after any non-MPI implementation 
# libraries
#
if [ "$linking" = yes ] ; then
    # Attempt to encode rpath info into the executable if the user has not
    # disabled rpath usage and some flavor of rpath makes sense on this
    # platform.
    # TODO configure and config.rpath are computing more sophisticated rpath
    # schemes than this simple one.  Consider updating this logic accordingly.
    if test "X$enable_wrapper_rpath" = "Xyes" ; then
        eval rpath_flags=\"${hardcode_libdir_flag_spec}\"
    else
	rpath_flags=""
    fi

    if [ "$nativelinking" = yes ] ; then
        $Show $F77 $PROFILE_INCPATHS ${final_fflags} ${final_ldflags} $allargs -I$includedir
        rc=$?
    else
        if [ "$static_mpi" = no ] ; then
          $Show $F77 $PROFILE_INCPATHS ${final_fflags} ${final_ldflags} "${allargs[@]}" -I$includedir -L$libdir $f77libs $PROFILE_PRELIB $PROFILE_FOO $rpath_flags -lmpi  $PROFILE_POSTLIB ${final_libs} 
        else
          fabric_dep=""
          if [ "" = yes ] ; then
              fabric_dep=`pkg-config --static --libs $libdir/pkgconfig/libfabric.pc`
              fabric_dep=`echo $fabric_dep | sed 's/-lfabric//'`
          elif [ -f /lib/pkgconfig/libfabric.pc ] ; then
              fabric_dep=`pkg-config --static --libs /lib/pkgconfig/libfabric.pc`
          else
              fabric_dep=`pkg-config --static --libs libfabric`
          fi
          $Show $F77 $PROFILE_INCPATHS ${final_fflags} ${final_ldflags} "${allargs[@]}" -I$includedir -L$libdir $f77libs $PROFILE_PRELIB $PROFILE_FOO $rpath_flags $libdir/libmpi.a  $PROFILE_POSTLIB ${final_libs}  ${fabric_dep}
        fi
        rc=$?
    fi
else
    $Show $F77 $PROFILE_INCPATHS ${final_fflags} $allargs -I$includedir
    rc=$?
fi
if [ -n "$rmfiles" ] ; then
    for file in $rmfiles ; do
        objfile=`basename $file .f`
	if [ -s "${objfile}.o" ] ; then
	    # Rename 
	    destfile=`echo $objfile | sed -e "s/.*$$-//"`
	    mv -f ${objfile}.o ${destfile}.o
	fi
        rm -f $file
    done
    rm -f $rmfiles
fi
exit $rc
