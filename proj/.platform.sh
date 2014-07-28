#!/bin/sh

[ "`uname -a|grep x86_64`" != "" ] && BUIILD_X64=ENABLED
[ "`uname -a|grep WOW64`" != "" ] && BUIILD_X86=ENABLED
