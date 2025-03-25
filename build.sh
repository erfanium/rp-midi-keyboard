set -e

cd build
cmake ..
make

# Wait for the device to be ready before copying
while [ ! -d "/media/erfanium/RPI-RP2" ]; do
    echo "Waiting for RPI-RP2 to mount..."
    sleep 1
done

cp ./rp-midi.uf2 /media/erfanium/RPI-RP2
sync  # Ensure the write is completed

echo "Deployed!"


# Wait for /dev/ttyACM0 to be available
while [ ! -e "/dev/ttyACM0" ]; do
    echo "Waiting for /dev/ttyACM0..."
    sleep 1
done

cat /dev/ttyACM0
