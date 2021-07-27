USAGE

window section:

    esc                         exit
    tab move                    move window
    shift +                     resolution up
    shift -                     resolution down

    windows_key f8              restore focus on window

alpha mode:
    
    f1                          enable/disable alpha mode
    windows_key ctrl f8         toggle alpha

image section:

    left mouse click 	    select image; -- if multiple images are on top of each other
                                selection will be applied to the next one in "queue"
    ctrl + move                 palette move
    
selected image: - selected image is highlighted in red
    
    ctrl + move                 image move
    left alt + move             ratio resize (small step) - 0.05% per pixel moved
    left alt + shift + move     ratio resize (large step) - 0.1% per pixel moved
                                // if you move your cursor up+left it shrinks,
                                otherwise it grows. Preview is shown in white.
    
    backspace                   reset ratio to 1.0
    scrollwheel                 change zindex / up +1, down -1
    delete                      remove image		
    
