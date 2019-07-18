#!/bin/sh
docker run -it -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix \
	-e XDG_RUNTIME_DIR=/tmp \
	-e QT_X11_NO_MITSHM=1 \
	-e WAYLAND_DISPLAY=$WAYLAND_DISPLAY \
	-v $XDG_RUNTIME_DIR/$WAYLAND_DISPLAY:/tmp/$WAYLAND_DISPLAY  \
	--device /dev/dri/card0:/dev/dri/card0 \
	--device /dev/dri/renderD128:/dev/dri/renderD128 \
	--rm=true $@
