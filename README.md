# blusb
Terminal based application to configure the mblusb controller for IBM Model M keyboards

Jörn Giest has created a replacement controller for the IBM Model M keyboard, for more information see the deskthority topic 
[IBM Model M BT-USB controller conversion kits](https://deskthority.net/viewtopic.php?f=55&t=17388)

Based on a Win32 commandline application written by Jörn to support his controller.

## How to install

Clone the repository and compile.

## How to operate

The controller can be configured in several ways. 

1. The controller supports six layers and each layer can have its own mappings.
2. Macros can be defined and assigned to keys.
3. The debounce periode can be changed.

You'll need two keyboards to configure the controller. The first obviously will need to be the model 
M keyboard, this keyboard will be used to locate a key on the matrix. Upon pressing a key on this keyboard 
the corresponding matrix cell will be selected.

Using the second keyboard one can navigate the matrix using the arrow keys, and also enter values or select from a popup.

