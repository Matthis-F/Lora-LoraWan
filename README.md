# LoRa/LoRaWAN Gateway Project

Intened as a school project with the goal of creating a Low-Cost __Single Channel__ LoraWAN Gateway software for a Raspberry Pi, this Project also implement __routing__ between the Gateway and the Node 

__! This repo is no longer supported !__

This is not a tutorial but if you read everything and follow all instructions you should be able to make everything work

# Hardware Requirements

  ### Raspberry Pi
  
  The provided software was only tested on a __Raspberry Pi__ 3B and __3B+__ using __Raspbian__ !
  
  Although since the BCM2835 library is compatible with all raspberry pi version (including zero and Pi 4) it should work on any raspbery pi
  
  ### Lora Module
  
  The provided software was only tested on the __HopeRF RFM95__ Module with [this](https://www.tindie.com/products/m2m/1-channel-lorawan-gateway-shield-for-raspberry-pi4/) sheild
  
  But since the RadioHead (RH_RF95 Driver) library is compatible with others modules it should work on :
  
    - Semtech SX1276/77/78/79
    - Modtronix inAir4 and inAir9
    - HopeRF RFM96/97/98
  
   If your radio module is not listed above there's still hope you can make this work, this software is only using the RH_RF95 Driver but RadioHead supports lots of other drivers,
   You may need to modify the source code and recompile it to work with your module and the correct driver see [here](https://www.airspayce.com/mikem/arduino/RadioHead/) for more informations
  
  ### Arduinos (Lora End Nodes)
  
   We only tested codes given in the Arduino folder, it should work on all Arduinos, as long as they have a LoRa module supported by the RadioHead RH_FR95 driver (see list above),
   
   We used an [adafruit Feather 32u4](https://www.adafruit.com/product/3078) with a RFM95 LoRa module on it. 
   If you are considering buying arduino(s) for ceating a Lora/LoraWAN infrastructure and 
   considering using routing, RadioHead recommend at least 2kB of RAM,  even if it works with theses adafruit feather 32u4 you will be limited by the number of routing entries.
   
   
  # Software Requirements
  
   The provided software was only tested using the BCM2835 Library __version 1.64__
   
   And the RadioHead library from hallard github link [here](https://github.com/hallard/RadioHead)
   
   I'm aware that newer versions doesn't use the bcm2835 lib anymore and switched to pigpio, the gateway was not meant to work with pigpio,
   this is why it __doesn't work with newer versions of RadioHead__ make sure you are using the one from hallard's repo.
   
   To this date there's issues with pigpio and RadioHead, it looks like there is race conditions between threads, this is why I'm not using it. 
   It is possible theses issues have been corrected since this was made.
   
# How does it work ?
## Tested Infrastructure
Here's a diagram of the tested infrastructure implemented in this project :
                                                                                                    
                                                           +----------------+            +----------------+
    +-------------+            +------------+              |RPI             |            |                |
    |Arduino      |            |Arduino     |              |                |            |+--------------+|
    | (End Node)  | <--------> |  (Router)  | <----------> |    Gateway     | <--------> ||LoraWan Server||
    |     Node 100|     ^      |    Node 10 |      ^       |                |     ^      |+--------------+|
    +-------------+     |      +------------+      |       |          Node 1|     |      |                |
                      LoRa                       LoRa      +----------------+     |      |                |
                   Modulation                 Modulation                      Internet   |                |
                                                                                (IP)     |                |
                                                                                         |                |
                                                                                         |                |
                                                                                         +----------------+
                                                                                     
## How does the Gateway works ?

A LoRa/LoRaWAN gateway will forward received packets sent by nodes to a LoRaWAN server
                                                                                          
                Lora Radio    +-----------------------+  The Internet                         
                Modulation    |                       |                                       
                              |                       |                                       
      Incoming                |                       |                                       
      Packets  ----------->   |        Gateway        |  -----------> Server(s)               
                              |                       |                                       
                              |                       |                                       
                              |                       |                                       
                              +-----------------------+                                       
To get packets received by the Lora Module it uses the RadioHead library.
To be able to send and receive packets RadioHead provides a Driver and a manager.

The driver is specific to the LoRa module, since we are using the HopeRF RFM95 module in our project we used the RH_RF95 driver.
The manager is always the same, it enables routing and adressed packets (and much more if you want !)

### How packets are sent on Lora Modulation ?
Here's how RadioHead format a packet before sending it :

              Header                   
    --------------------------        
    v                        v        
                                     
    +--------------------------------------------------------------------------------------------------+
    | To | From | ID | Flags |                              Payload                              | CRC |
    +--------------------------------------------------------------------------------------------------+
                8 Bytes                                  0 - 251 Bytes
    
    To : The address of the recipient node
    From : The address of the node sending the packet
    ID : 8 bytes value that can be whatever you want usually a framecounter
    FLAGS : 8 bytes value that can be whatever you want
    
See [here](https://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF95.html) for more details

### How Packets are sent to the Internet ?

Once we have read our packet from the module we need to send it to the Server(s), to make the LoraWAN server(s) understand what we are sending them we need to use a protocol,
this gateway is using the [Semtech UDP Packet Forwarder](https://github.com/Lora-net/packet_forwarder) Protocol. Despite being old and based on UDP it's simple and is currently supported
by all LoraWAN servers.

### What about LoRaWAN

__The packet you send needs to be formatted according to the [LoraWAN specifications](https://lora-alliance.org/resource-hub/lorawanr-specification-v103), if not they will be rejected by the server.__

__End Nodes (Arduinos) must be configured correcly__


# How to install it ?

## __Using the setup script__ 

First you will need to clone this repo :

    git clone https://github.com/Matthis-F/Lora-LoraWan.git

Then install the Gateway by executing the setup script inside the Lora-LoraWAN folder :
    
    ./setup

## Install it Manually
  
  __Unless the setup script fails, I highly recommend using it__
  
  ### 1. Install the BCM2835 Library
  
  First you need to install the dependencies for the BCM2835 library :
    
    sudo apt-get install libcap2 libcap-dev
  
  Then add the user to the kmem group :
    
    sudo adduser $USER kmem
    echo 'SUBSYSTEM=="mem", KERNEL=="mem", GROUP="kmem", MODE="0660"' | sudo tee /etc/udev/rules.d/98-mem.rules
  
  Finally [download](https://www.airspayce.com/mikem/bcm2835/), extract and compile the library :
    
    wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.64.tar.gz
    tar -xvf bcm2835-1.64.tar.gz
    cd bcm2835-1.64/
    ./configure
    make
    sudo make check
    sudo make install

   ### 2. Install the Gateway and the RadioHead library
    
  First Download this repo:
    
    git clone https://github.com/Matthis-F/Lora-LoraWan.git
    
  Then Download the RadioHead Library
  
    git clone https://github.com/hallard/RadioHead.git
    
  Place the Radiohead library in the Gateway folder :
  
    mv RadioHead Lora-LoraWAN/Gateway/ 

  Compile the Gateway software
  
    cd Lora-LoraWAN/Gateway/
    make
    
  ### 3. Setting up the Raspberry
  
  __You need to enable SPI access on the raspberry__ :
  
    sudo raspi-config
  
  Go to __Interfacing Options__ and enable __SPI__ 
  
  __You also might need to add the following line to the config.txt file__ in /boot directory :
    
    sudo su
    echo "dtoverlay=gpio-no-irq" >> /boot/config.txt
  
# How to configure it ?

  Once you have installed the gateway you will need to configure it
  
  There is a configuration file named __config.gw__ in the Gateway folder of the project
  
  Use this file to configure :
  
    - Your Lora module pins
    - Radio Frequency Setting to match your local regulation
    - Radio Power Setting to match your local regulation
    - LoraWAN Server(s)
    - Routes for accessing Nodes

  Default values are working with [this](https://www.tindie.com/products/m2m/1-channel-lorawan-gateway-shield-for-raspberry-pi4/) sheild
  and match France Regulations
  
  __If you are not located in France check what frequency and power value you are allowed to use before using the Gateway__
  
  If your module can take advantage of PA_BOOST you can set the Power value up to 23dBm max if not the maximum uis 14dBm
  
  
# How to use it ?

To start the gateway server just launch the rf95_server script :
    
    sudo ./rf95_server

Unless you are root you will need super user permissions to access the GPIO

# It doesn't work what can I do ?

## 1. The installation has failed

If the installation fails using the script you may want to try to build it manually (see installation part)
Libraries may have been updated and links broken, trying to download them manually may fix the issue

Make sure you have access to Internet and you have a proper DNS configuration

You may also try to install it as root

## 2. Compilation of rf95_server.cpp has failed

It is possible that either the bcm2835 or the RadioHead library are causing the problem, check it bcm2835 was correctly installed and compiled, and if the radiohead lib is placed in the Lora-LoraWAN/Gateway/ directory

If the error seems to come from somewhere else try googling it, and try to fix it yourself (Everything has been tested and works see Hardware and Software requirments for more info) 

## 3. Weird crashes

Check if the SPI Interface is active on the Raspberry and try to change the dt_overlay value in /boot/config.txt (see manual inslattaion Part 3.)

## 4. Errors once the gateway server lauched

### Segmentation fault ###
  
  The program is probably trying to access something it cannot access, make sure you launched it with super user permissions
  
  In most files there is a preprocessor #ifdef DEBUG commented, uncomment it for each file it and recompile the code for more debugging infos

### Other errors or warnings ###
  
  Most errors give you possible instructions on how to solve them, try to follow them
  
  Double check everything if it worked before try reinstalling everything
  
  
## 5. Something else

If you are stuck you can still try to open an issue, maybe someone with the same problem can help you

I don't support this repo anymore because I don't have the required hardware anymore, I can't garuantee you any solutions to your problems.

# Liscence

__Do whatever you want with it !__

Some of the libraries included in this repository are included for convenience only and all have their own license, and are not part of this project.
