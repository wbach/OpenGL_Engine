clc
clear all
close all
img = imread('Plans4.png');
img = imresize(img, [4096 4096]);
imshow(img)
main_roads = img(:, :, 1);
main_roads( main_roads < 255) = 0;
imshow(main_roads)

[w h c] = size(img); 
small_roads = zeros(w, h);
trees = zeros(w, h);
fields = zeros(w, h);
for i =1 : w
   for j = 1 : h
       if( img(i, j, 1) > 250 &&  img(i, j, 2) > 240)
           small_roads(i, j) = 255;
       end
       if( img(i, j, 1) < 250 &&  img(i, j, 2) > 240 && img(i, j, 3) < 250)
           trees(i, j) = 255;
       end
       if( img(i, j, 1) < 250 &&  img(i, j, 2) > 240 && img(i, j, 3) > 250)
           fields(i, j) = 255;
       end       
   end
end

%trees( trees(:, :, 1) < 250 &&  trees(:, :, 2) < 250 ) = 0;
%trees(  < 255) = 0;
imshow(small_roads)
imshow(trees)
imshow(fields)

blend_map = uint8(zeros(w, h, 3));
blend_map(:,:,1) = main_roads;
blend_map(:,:,2) = trees;
blend_map(:,:,3) = fields;
imwrite(blend_map, 'blend_map.png');
H = fspecial('disk',10);
blur_blend_map = imfilter(blend_map, H,'replicate');
imshow(blur_blend_map)
