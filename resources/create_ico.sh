#!/usr/bin/env bash

# https://docs.microsoft.com/en-us/windows/apps/design/style/app-icons-and-logos
sizes=("16" "24" "32" "48")
src_icon="capslock.svg"

# When set to true, will create icons with true white and black colors
use_true_color=false

icon_name="capslock_"

# For use with imagemagick convert
ico_b_name=""
ico_w_name=""

for size in ${sizes[@]}; do
    if [ $use_true_color = false ]; then
        inkscape -e ${icon_name}${size}"b.png" -i "dark" -j capslock.svg -z -w ${size} -h ${size}
        inkscape -e ${icon_name}${size}"w.png" -i "light" -j capslock.svg -z -w ${size} -h ${size}
    else
        inkscape -e ${icon_name}${size}"b.png" -i "black" -j capslock.svg -z -w ${size} -h ${size}
        inkscape -e ${icon_name}${size}"w.png" -i "white" -j capslock.svg -z -w ${size} -h ${size}
    fi
    ico_b_name="${ico_b_name} ${icon_name}${size}b.png" 
    ico_w_name="${ico_w_name} ${icon_name}${size}w.png" 
done

convert ${ico_b_name} -dither None ${icon_name}"b.ico"
convert ${ico_w_name} -dither None ${icon_name}"w.ico"

rm *.png
mkdir -p ./icons
mv *.ico ./icons