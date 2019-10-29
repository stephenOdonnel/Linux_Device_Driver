
module="EnsiCaen_ldd"
device="EnsiCaen_Device"

sudo rmmod $module
sudo rm -f /dev/$device
