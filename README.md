Implementation for the hackaton Mouve Your Robot '17
====================================================


Building
--------

```
$ git submodule update --init --recursive
$ mkdir build
$ cd build
$ cmake ..
$ make
```


Troubleshooting
---------------

If the following error appears:

```
In file included from ApiCodec/src/vitals/CLException.cpp:22:0:
ApiCodec/src/vitals/Format.hpp: In static member function ‘static const char* tinyformat::detail::FormatIterator::streamStateFromFormat(std::ostream&, unsigned int&, const char*, int, int)’:
ApiCodec/src/vitals/Format.hpp:786:21: error: this statement may fall through [-Werror=implicit-fallthrough=]
```

Fix it by removing the `-Werror` flag from the file `ApiCodec/CMakeLists.txt`, line 35.


Running
-------

At project's root, do:
```
./bin/EpitechClient
```
