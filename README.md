A simple tool to set the backlight using /sys/class/backlight
=============================================================

On my DELL-Laptop xbacklight returns after a delay of several seconds. So I 
decided to write a simple program to set the backlight using the media-keys.

The program must run under root. You could do a

  sudo chown root:root backlight
  sudo chmod +s backlight

In my i3-conf I have the following lines

  bindsym XF86MonBrightnessUp exec "backlight --inc=10 intel_backlight"
  bindsym XF86MonBrightnessDown exec "backlight --dec=10 intel_backlight"



