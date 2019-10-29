
module="EnsiCaen_ldd"
device="EnsiCaen_Device"

# install the LKM and exit if insmod fails with an error
sudo insmod $module.ko verbose=1

# query the /proc/devices file
major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
minor=0

echo major:$major

# create the new file system node
sudo mknod /dev/$device c $major $minor

# ensure device file is readable by all
sudo chmod 666 /dev/$device
