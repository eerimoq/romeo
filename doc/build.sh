#!/usr/bin/env bash

set -e

dot -Tpng -Gsize=15\! -Gdpi=100 -o robot_state_machine.png robot_state_machine.dot
dot -Tpng -Gsize=3.5\! -Gdpi=100 -o robot_state_machine_cutting.png robot_state_machine_cutting.dot
dot -Tpng -Gsize=3.5\! -Gdpi=100 -o robot_state_machine_searching.png robot_state_machine_searching.dot

# export slides to pdf and then convert them to jpg
sudo soffice --headless --convert-to pdf schematics.odp
convert -density 150 schematics.pdf -quality 85 schematics.jpg

# crop exported slides
convert schematics-1.jpg -crop 1654x1240+0+200 robot_schematics.jpg
convert schematics-2.jpg -crop 1654x1000+0+210 base_station_schematics.jpg
convert schematics-3.jpg -crop 1654x1000+0+230 connectivity.jpg
convert schematics-4.jpg -crop 1654x700+0+350 controller_smartphone_app.jpg

rm -f schematics.pdf
rm schematics-*.jpg
