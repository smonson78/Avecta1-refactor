#!/bin/bash
export ASAN_OPTIONS=abort_on_error=1
export GDK_BACKEND=x11
export SDL_VIDEODRIVER=x11 

./test
