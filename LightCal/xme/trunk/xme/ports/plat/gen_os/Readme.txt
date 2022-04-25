Generic implementations of functionality which requires platform / OS support.
It generally needs a fully-fledged C-library (e.g. assert(), printf(), and
especially support for dynamic memory management like malloc()).

This folder intentionally does not contain a CMakeLists.txt since its
files are intented to be included by the correspondig platforms.
