### Chris Pilkington
Copyright Â©
http://chris.iluo.net/

### License

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

### About

A simple music player because I am sick of Rhythmbox doing everything I don't care about and nothing that I do.
[Website][1]

### Building on Linux (Ubuntu 11.10 used)

Required applications:
cmake

Required libraries:
boost
clastfm
gnome-keyring
gstreamermm
gtkmm
id3tag
mad
xdg-basedir

Steps:
git clone git://github.com/pilkch/library.git
git clone git://github.com/pilkch/medusa.git
cd medusa/project/
cmake .
make
cd ..
cp project/medusa medusa
./medusa

### Credit

This application is created by Christopher Pilkington.
I only use libraries that are license under either the GPL or similar and am eternally grateful for the high quality ease of use and generosity of the open source community.

gstreamermm
Copyright 2008, The gstreamermm Development Team
All rights reserved
[gstreamermm][2]

gtkmm
Copyright 2002, gtkmm development team
All rights reserved
[gtkmm][3]

 [1]: http://chris.iluo.net/projects/medusa
 [2]: http://live.gnome.org/GStreamer
 [3]: http://live.gnome.org/gtkmm

