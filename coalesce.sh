for f in frame_*.pbm; do
  # convert $f -filter point -resize 1000% -negate -virtual-pixel edge -channel R -fx "p[-1,-2]" -channel B -fx "p[2,1] " "aberration_$f"

  # convert $f -filter point -resize 1000% -negate -virtual-pixel edge -channel R -fx "p[-1,-2]" -channel B -fx "p[2,1]" "resize_$f"

  fbase=$(echo $f | sed 's/\..*//g')

  convert $f -filter point -resize 1000% -negate "$fbase.jpg"

  # convert "resize_$f.png" -write MPR:orig +delete \
  # \( MPR:orig -separate -delete 1,2 -affine -1.00,0,0,-1.00,0,0 -transform -gravity Center -extent 210x240 \) \
  # \( MPR:orig -separate -delete 0,2 -affine -1.06,0,0,-1.06,0,0 -transform -gravity Center -extent 210x240 \) \
  # \( MPR:orig -separate -delete 0,1 -affine -1.31,0,0,-1.31,0,0 -transform -gravity Center -extent 210x240 \) \
  # -combine "aberration_$f.png"

  convert "$fbase.jpg" -write MPR:orig +delete \
  \( MPR:orig -separate -delete 1,2 -affine 1.00,0,0,1.00,0,0 -transform -gravity Center -extent 210x240 \) \
  \( MPR:orig -separate -delete 0,2 -affine 1.02,0,0,1.02,0,0 -transform -gravity Center -extent 210x240 \) \
  \( MPR:orig -separate -delete 0,1 -affine 1.04,0,0,1.04,0,0 -transform -gravity Center -extent 210x240 \) \
  -combine "aberration_$fbase.jpg"

  rm "$fbase.jpg"
done

convert -delay 5 aberration* final.gif
