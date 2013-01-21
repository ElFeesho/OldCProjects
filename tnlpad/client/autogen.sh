srcdir=`dirname $0`
thisdir=`pwd`
srcdir=`cd $srcdir && pwd`

(cd $srcdir && autoreconf --install)
