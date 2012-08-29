#!/bin/sh
# Generate version automatically.

version=$(git describe --match "v[0-9]*" | sed -e 's/-/./g')
echo "COBJ_VERSION = $version"
