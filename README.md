Implementation for the hackaton Move Your Robot '17
====================================================


Building
--------

Simply run the following command:

```sh
$ ./build.sh
```

Or to build it step-by-step:

```sh
$ git submodule update --init --recursive
$ cd ApiCodec
$ patch -p1 --forward < ../patch/ApiCodec.patch
$ cd ..
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
```


Running
-------

At project's root, do:
```sh
$ ./bin/EpitechClient
```
