#!/bin/sh -e

fatal()
{
	printf %s\\n "${0##*/}: $*" >&2
	exit 1
}

[ $# -eq 1 ] || fatal 'Please specify the name of the directory containing gnulib'

[ -d "$1" ] || fatal "$1 is not a directory"

tool="$1/gnulib-tool"
[ -f "$tool" -a -x "$tool" ] || fatal "$tool is not available"

mkdir -p doc lib m4
"$tool" --import --dir=. getdate
