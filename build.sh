#!/bin/sh

set -euo pipefail

print_fatal(){
	printf "[31;1m::[37;1m %s[m\n" "$@" >&2
	exit 1
}

print_step(){
	printf "[33m =>[m %s\n" "$@"
}

print_msg(){
	printf "[34m::[m %s\n" "$@"
}

choice(){
	read -r -p "$(print_msg "$1 (Y/n): ")" -n 1 input
	test "${input}" = "$(echo)" || echo >&2
	if test "${input^^}" = 'N'
	then echo "$3"
	else echo "$2"
	fi
}

build(){
	print_msg 'Preparing project.'
	if test -d ./.git ; then
		print_step "Fetching ApiCodec submodule."
		git submodule update --init --recursive || print_fatal "Failed to fetch ApiCodec!"
	fi
	if test -d ./build ; then
		if test "${1:-}" = '-f' || test "${1:-}" = '--force' ; then
			print_step "Removing directory ./build. [--force]"
			rm -r -f ./build
		else
			print_fatal 'Directory ./build already exists! Requires [-f | --force] flag.'
		fi
	fi
	mkdir ./build
	cd ./build
	print_step 'Setting build options.'
	local patch_ApiCodec="$(choice 'Apply ApiCodec build patch (patch/ApiCodec.patch)?' 'true' 'false')"
	local cmake_build_type="$(choice 'Use Debug build type?' 'Debug' 'Release')"
	print_msg 'Building project'
	if ${patch_ApiCodec} ; then
		print_step 'Patching ApiCodec submodule.'
		cd '../ApiCodec/'
		git checkout .
		patch -p1 --forward --force < '../patch/ApiCodec.patch' || print_fatal 'Failed to apply patch/ApiCodec.patch!'
		cd '../build/'
	fi
	print_step 'Executing cmake command.'
	cmake -DCMAKE_BUILD_TYPE="${cmake_build_type}" .. || print_fatal 'CMake failed!'
	print_step 'Building project.'
	make || print_fatal 'Failed to build project!'
	print_msg "Build complete!"
}

build "$@"
