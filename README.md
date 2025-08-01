# oled-time
README :- SSD1306 0.91 OLED Display This is a simple C Program code that Helps you Print Time on the Oled. The first version displays time in HH::MM::SS format.I've included the freedom of choosing font by including headers and custom fonts made by editing the hex code. I've also included the arm binary file to directly convert to executable on the luckfox.

prequisites:

    Luckfox Pico (Mini B) connected over USB/LAN.
    Cross-compiler toolchain installed (arm-linux-gnueabihf-gcc).
    IP address of the Luckfox board.
    SSH/SCP access enabled on the board.

Steps : ( This is done in the host machine, in my case its an ubuntu linux machine and my target is an ARM based SoC)

    Navigate to Desired Directory using "cd home/your/path" .
    Create the c file using " nano filename.c " write/paste the above code using nano terminal and press ctrl+o (to save) and ctrl+x (to exit).
    Compile the code for arm based target i.e luckfox using i.e "arm-linux-gnueabihf-gcc -static filename.c -o filename"(to be created) I use -static as my target OS is minimal buildroot and lacks dynamic libraries. The command converts the c code to ARM based C file.
    Then using using SCP command transfer the ARM_C file to luckfox pico using the command "SCP filesname root@:/<dest. addr>.

to be done on the luckfox using terminal(connect the luckfox device on the host machine and get the ip address of the device)

    Login to the Luckfox using ssh, i.e "ssh root@ip address"
    Change directory to the dest. addr. then using chmod +x filename change the file permission i.e change the arm_c file to executable to run code on luckfox.
    Then using command ./filename run the executable on luckfox.

