o_files=""

for c_file in $(find src -name "*.c"); do
	c_name=$(basename $c_file)
	o_file="build/$(echo $c_name | cut -d "." -f1).o"
	echo "[gcc] Compiling $c_name to $o_file"
	o_files="${o_files} \"$o_file\""
done

echo $o_files