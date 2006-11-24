#! /bin/sh
ditto -arch ppc LDView.app/Contents/MacOS/LDView LDView.app/Contents/MacOS/LDView.ppc
mv LDView.app/Contents/MacOS/LDView.ppc LDView.app/Contents/MacOS/LDView
