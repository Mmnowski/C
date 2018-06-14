<h1>Xlib RGB mixer - concurrent computing project</h1>

Uses shared memory to store RGB data, which is later managed between clients.

Usage: ./compile.sh, then go into bin, launch server and then client

Connecting through ssh requires -X option and providing env variable $DISPLAY to the client (eg. "./client localhost:12.0").

