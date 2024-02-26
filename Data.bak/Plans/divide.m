clc
clear all
close all
img = imread('blend_map_blur.png');
[w h c] = size(img); 

dsize = 205;%128;
mkdir('BlendMaps');
ii = w /dsize;
jj = h / dsize;

for i = 0: ii -1
    for j = 0 : jj -1
        a = i * dsize + 1;
        b = j * dsize + 1;

        partimg = img(a : a+dsize - 1,  b : b+dsize - 1, : );
        imwrite(partimg, ['BlendMaps/terrain_Terrain'  num2str(i-1)  'x' num2str(j-1) '.png']);
    end
end

