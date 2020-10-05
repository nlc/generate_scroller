file_name='17lnfxj9p8i11.jpg'
file_dim='1280x959'

convert $file_name -write MPR:orig +delete \
\( MPR:orig -separate -delete 1,2 -affine 1.00,0,0,1.00,0,0 -transform -gravity Center -extent $file_dim \) \
\( MPR:orig -separate -delete 0,2 -affine 1.05,0,0,1.05,0,0 -transform -gravity Center -extent $file_dim \) \
\( MPR:orig -separate -delete 0,1 -affine 1.10,0,0,1.10,0,0 -transform -gravity Center -extent $file_dim \) \
-combine "aberration_$file_name"
