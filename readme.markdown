### Chris Pilkington
Copyright Â©  
http://chris.iluo.net/

If you find this project helpful, please consider making a donation. 

[<img alt="Make a donation via Pledgie" src="http://www.pledgie.com/campaigns/17973.png?skin_name=chrome" border="0" />][1]  

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
[Website][2]

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
./medusa

### Credit

This application is created by Christopher Pilkington.  
I only use libraries that are license under either the GPL or similar and am eternally grateful for the high quality ease of use and generosity of the open source community.

gstreamermm  
Copyright 2008, The gstreamermm Development Team  
All rights reserved  
[gstreamermm][3]

gtkmm  
Copyright 2002, gtkmm development team  
All rights reserved  
[gtkmm][4]


 [1]: http://www.pledgie.com/campaigns/17973
 [2]: http://chris.iluo.net/projects/medusa
 [3]: http://live.gnome.org/GStreamer
 [4]: http://live.gnome.org/gtkmm

