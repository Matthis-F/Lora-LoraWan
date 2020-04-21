
#####BCM 2835 Library######

cd libraries

sudo apt-get install libcap2 libcap-dev #Installing dependencies

#Adding user to kmem group##
sudo adduser $USER kmem
echo 'SUBSYSTEM=="mem", KERNEL=="mem", GROUP="kmem", MODE="0660"' | sudo tee /etc/udev/rules.d/98-mem.rules

#Downloading library
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.64.tar.gz 

#Extracting library
tar -xvf bcm2835-1.64.tar.gz
rm -f bcm2835-1.64.tar.gz* #Cleaning unused files

cd bcm2835-1.64/

./configure #Configuring library
make #Compiling library
sudo make check #Running tests
sudo make install #Installing library
#############################

##### RadioHead Library #####
cd ../../Gateway
#Downloading library
git clone https://github.com/hallard/RadioHead.git
#############################


######## RF95 Server ########
make #Compiling  rf95 server
#############################
