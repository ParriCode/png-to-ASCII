# png_to_ASCII
Convert png images to ASCII images. you can rescale the images and modify the characters that are used to draw the image

```
cd Release
make all
./png_to_ASCII --help
```
## How to use 
execute png_to_ASCII and them :
1. specify a directory to PNG image
2. specify a directory to save text file
3. specify what characters configuration yo you eant to use
4. (optional) width and height to rescale image. Default the image will be converter with image original size

### example
```
./png_to_ASCII images/tux.png ascii_images/tux.txt config1 300 200
```

## modify characters
```
./png_to_ASCII --config [data-file]
```
This command execute a program that help to edit data characters
If the data file doesn't exist they create a new file
The files was in the /data folder and you can modify it manualy or with the program.

## Warning
To open the images converted to ascii it is recommended to have a text editor configured for it. The font must be regular and all the characters occupy the same space, otherwise the image will be crooked
