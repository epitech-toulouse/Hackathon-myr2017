Implementation for the hackaton Move Your Robot '17
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

If the following errors appears:

```
In file included from ApiCodec/src/vitals/CLException.cpp:22:0:
ApiCodec/src/vitals/Format.hpp: In static member function ‘static const char* tinyformat::detail::FormatIterator::streamStateFromFormat(std::ostream&, unsigned int&, const char*, int, int)’:
ApiCodec/src/vitals/Format.hpp:786:21: error: this statement may fall through [-Werror=implicit-fallthrough=]
```

Fix it by removing the `-Werror` flag from the file `ApiCodec/CMakeLists.txt`, line 35.

```
myr2017/ApiCodec/src/ApiCodec/HaGpsPacket.cpp:13:1: error: prototype for ‘HaGpsPacket::HaGpsPacket(ulong, double, double, double, uint8_t, uint8_t, uint8_t, double)’ does not match any in class ‘HaGpsPacket’
 HaGpsPacket::HaGpsPacket( ulong time_, double lat_, double lon_, double alt_, uint8_t unit_, uint8_t satUsed_, uint8_t quality_,double groundSpeed_ )
```
Fix it by changing the file `ApiCodec/src/ApiCodec/HaGpsPacket.cpp`, line 13 from `ulong time_` to ``uint64_t time_.


Running
-------

At project's root, do:
```
./bin/EpitechClient
```
