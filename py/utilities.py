"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import sys
import platform
import time
import csv

zmq = None

# ---------------------------------------------------------------------------------------------------------------------
def check_for_zmqpy():
    enable_zmqpy = 1
    #
    # SET VS90COMNTOOLS=%VS100COMNTOOLS%
    # C:\Python27\python.exe -O lcbt.py
    # zmqpy does not work on windows
    # C:\Python27\lib\site-packages\zmqpy\__pycache__\_cffi__x88b86722x954aa029.c(153) :
    #  fatal error C1083: Cannot open include file: 'zmq.h': No such file or directory
    #
    if sys.platform == 'win32' or sys.platform == 'cygwin':
        enable_zmqpy = 0

    if enable_zmqpy:
        global zmq
        try:
            import zmqpy as zmq
        except ImportError:
            zmq = None
            sys.stderr.write("Did not find zmqpy module installed, disabling parallel workers\n")
            enable_zmqpy = 0

    #if enable_zmqpy:
    #    print("Found zmqpy")

    return enable_zmqpy


#----------------------------------------------------------------------------------------------------------------------
def check_for_sqlite():
    if platform.python_implementation() == 'PyPy':
        enable_sqlite = 0
    else:
        enable_sqlite = 1
    return enable_sqlite

def check_for_pyzmq():
    enable_pyzmq = 1

    if platform.python_implementation() == 'PyPy':
        enable_pyzmq = 0

    if enable_pyzmq:
        global zmq
        try:
            import zmq
        except ImportError:
            zmq = None
            sys.stderr.write("Did not find pyzmq module installed, disabling parallel workers\n")
            enable_pyzmq = 0

    #if enable_pyzmq:
    #    print("Found pyzmq")

    return enable_pyzmq


class Unbuffered(object):
    def __init__(self, stream):
        self.stream = stream

    def write(self, data):
        self.stream.write(data)
        self.stream.flush()

    def __getattr__(self, attr):
        return getattr(self.stream, attr)

if sys.version < '3':
    import codecs
    def u(x):
        return codecs.unicode_escape_decode(x)[0]
    def s(x):
        return x.encode('ascii')
else:
    def u(x):
        return x
    def s(x):
        return x.encode('ascii')

if sys.platform == "win32":
    # On Windows, the best timer is time.clock()
    default_timer = time.clock
else:
    # On most other platforms the best timer is time.time()
    default_timer = time.time


class Timer(object):
    def __enter__(self, timer=default_timer):
        self.__start = timer()

    def __exit__(self, _, value, traceback, timer=default_timer):
        # Error handling here
        self.__finish = timer()

    def duration_in_seconds(self):
        return self.__finish - self.__start


if sys.version_info.major < 3:
    from StringIO import StringIO
    stringio = StringIO
else:
    from io import StringIO
    stringio = StringIO

if sys.version_info.major < 3:
    import codecs

    class UTF8Recoder(object):
        """
        Iterator that reads an encoded stream and re-encodes the input to UTF-8
        """
        def __init__(self, f, encoding):
            self.reader = codecs.getreader(encoding)(f)

        def __iter__(self):
            return self

        def next(self):
            return self.reader.next().encode("utf-8")

    class UnicodeReader(object):
        """
        A CSV reader which will iterate over lines in the CSV file "f",
        which is encoded in the given encoding.
        """
        def __init__(self, f, dialect=csv.excel, encoding="utf-8", **kwds):
            f = UTF8Recoder(f, encoding)
            self.reader = csv.reader(f, dialect=dialect, **kwds)

        # Do this weird way to get at unicode so Eclipse's pyDev does not complain about undefined
        # function under python3.2
        # def next(self, unicode=__builtins__.__dict__['unicode']):
        def next(self):
            row = self.reader.next()
            return [unicode(s, "utf-8") for s in row]

        def __iter__(self):
            return self


class CLIError(Exception):
    """Generic exception to raise and log different fatal errors."""

    def __init__(self, msg):
        super(CLIError).__init__(type(self))
        self.msg = "E: %s" % msg

    def __str__(self):
        return self.msg

    def __unicode__(self):
        return self.msg


def download_data(url, file_name):
    if sys.version_info.major == 2:
        import urllib2 as urllib
        #import urlparse

        uo = urllib.urlopen(url)
        meta = uo.info()
        content_length = meta.getheaders("Content-Length")
        file_size = int(content_length[0]) if content_length else 0
    else:
        import urllib.request as urllib
        #import urllib.parse as urlparse

        uo = urllib.urlopen(url)
        content_length = int(uo.getheader("Content-Length", default=0))
        file_size = content_length

    # file_name = urlparse.parse_qs(urlparse.urlparse(url).query)['file'][0]

    if content_length:
        sys.stdout.write("Downloading: %s Bytes: %s\n" % (file_name, file_size))
    else:
        sys.stdout.write("Downloading: %s\n" % file_name)

    f = open(file_name, 'wb')
    file_size_dl = 0
    block_sz = 8192
    while True:
        data_buffer = uo.read(block_sz)
        if not data_buffer:
            break

        file_size_dl += len(data_buffer)
        f.write(data_buffer)
        if file_size:
            status = r"%10d  [%3.2f%%]" % (file_size_dl, file_size_dl * 100. / file_size)
        else:
            status = r"%10d" % file_size_dl
        status += chr(8) * (len(status) + 1)
        sys.stdout.write(status)

    f.close()
