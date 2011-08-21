#!/bin/sh

# Create configure script from configure.ac.
autoconf || exit 1

./configure $@