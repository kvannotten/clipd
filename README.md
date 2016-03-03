# clipd

A very simple clipboard manager for X11.

Currently it writes all CLIPBOARD events to: `/tmp/clipd.data`

# Build

Run `make`. This will give you the clipd executable that you can run.

# Usage

Run the clipd executable (currently it doesn't daemonize itself yet). Then you can query the file with any application you want. For example you could do something like: `tac /tmp/clipd.data | head -10 | rofi -dmenu | xclip -s c`, this will open a menu that allows you to select your last 10 clipboard entries and then put in in your CTRL+V register. 

# TODO

https://github.com/kvannotten/clipd/issues/1

# Credits

Code is based on work of http://www.habets.pp.se/
