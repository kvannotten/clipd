# clipd

## General

A very simple clipboard manager for X11. This application just listens to X11 clipboard events and stores all text that you put in the CLIPBOARD in a file. That's all it does, and it's all it should do!

## Build

Run `make`. This will give you the clipd executable that you can run.

## Usage

Run the clipd executable. You can set the storage path and the display to use; ie:

`clipd -d :0 -p /tmp/clipd.data`

If you do not set the display the default will be `:0`. If you do not set the path, the default will be: `/tmp/clipd.data`.

Afterwards you can query the file with any application you want. For example you could do something like: 

`tac /tmp/clipd.data | head -10 | rofi -dmenu | xclip -selection clipboard`

This will open a menu that allows you to select your last 10 clipboard entries and then put it in your ctrl+v register. 
You can however use any application or way you deem most desirable to query this file.

## Contributions

Fork it and add your desired functionality. Remember, however, that this has to stay lightweight and simple.

## Credits

Code is based on work of http://www.habets.pp.se/
