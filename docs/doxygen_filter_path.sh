
grep -v "$1" > /dev/null << EOF
$2
EOF
if [ $? -eq 0 ] ;then
	cat $2
fi
