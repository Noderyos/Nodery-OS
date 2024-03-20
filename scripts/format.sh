/usr/sbin/fdisk $1 << EOF
o
n
p
1


w
q
EOF

loop_dev="`sudo losetup -f`"

sudo losetup -P "$loop_dev" "$1"

sudo mkfs.fat -F16 "$loop_dev"p1

sudo losetup -d $loop_dev