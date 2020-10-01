**********If you just want to "play" download the "DEPLOY.zip" and run the .exe!**********



# Bulb

```
#: completed
?: testing
-: pending

todo:
#strafing
#setting sorting
#saving
#add movespeed and rotation speed to settings
#add hsv color setting changing
#in app other fractal loading & keep raytracer settings
#fullscreen
#dpad setting changing improvements
#setting changing expo
#move camera during setting changing
#fix shader menu light color text
#improve animate settings display
#improve vec2+ animate 
#move controlsettings into bulbsettings as pointer
#move gamepad update for exploring into controlsettings
#create folders for shaders fractals and saves
#add keyboard menu control
#add actual string tools code
?loading
-more fractals
-scale animation to proximity setting
#skyboxes (space, ocean, gradient)
-volumetric/atmospheric lighting/shadows 
-fix controller vibrate
#somehow un-fuck control variables (maybe take inspiration from shadervariables)
#fix dpad int adjustments
-add fonts and make menus pretty
-figure out better way of sending depth data from gpu other than alpha
-floating particles
#save file images (needs testing)
?better keyboard handling (needs testing)
#start by actually looking at fractal
-maybe add draw_bulbvar_menu() and draw_text_menu() functions to bulbsettings to simplify menu drawing
?fix dpad bool adjust
-make load bulbsaves menu look better
-allow for higher than screen scrolling in loadsaves menu
-add screenshots (already added BMP class)
#reset order of shader vars after load
#fix image saving
#add on screen fps counter (moved this to top right of screen)
#"select" exits menu everywhere
#save overwrite and delete
-reload current save
#camera_fov issue
-make animations and movement deltatime based instead of frame based
#add tooltips and menu instructions (basic tooltips added)
-variable super fine adjustment
-emergency escape fractal (if stuck somewhere)
#fixed bmp writing (bgr problem)
#save image hide UI
#save image screen res non multiple of 4
-fix biocube
#started implementing skybox from IBL-Raytracer
```


# BULB is developed by Justin Barrada.

It is HIGHLY recommended that you run this with a reasonably powerful GPU, raytracing is very computationally expensive.

It is also recommended that you "play" with an XBox controller, keyboard works too but this was intended to be "played" with an XBox controller. The controls aren’t optimized for keyboard.

"Play" is in quotations because this isn’t really a game, it is more of a sandbox and a good way to turn your GPU into a space heater during those chilly winter months.


## KEYBOARD CONTROLS
ESC 			- will bring up the menu or back you out of a menu if you are in one
ENTER			- (in menu) select
Arrow Keys 		- (in menu) menu navigation (out of menu) up/down is pitch and left/right is yaw
W/A/S/D 		- (out of menu) forward back and strafing
Q/E				- (out of menu) roll

## XBOX CONTROLS
Back 			- toggle full screen
Start 			- open/close menu
DPad 			- (in menu) menu navigation
A 				- (in menu) select
B 				- (in menu) back out of a menu
X				- (in menu) animate setting
Y				- (in menu) toggle HSV or RGB color
LStick 			- (out of menu) vertical movement and yaw
RStick 			- (out of menu) pitch and roll
LT 				- (out of menu) move backwards (in menu) value adjustment
RT 				- (out of menu) move forwards (in menu) value adjustment


## Points of Interest
You can load one of a few different "Fractals". These contain the core mathematics for the structure of the object. They each have their own settings that can be manipulated and animated. The default fractal is the "Cross Menger"
You can load and save "Saves" which contain the "Fractal" and any settings that were manipulated or animated. Think of these as a kind of save state. These files are saved as .BMP files.
The "Shader Settings" menu is where you can tweak and animate almost any setting, fog, detail, colors, and geometrics properties. Keep an eye out for the help tips in the top left of the screen to guide you along.


PLEASE keep in mind, this was made by one person in their off time. This is still technically a work in progress and will have bugs and maybe even crash. That said, if you do find any bugs or have any updates you would like to see, please let me know!
