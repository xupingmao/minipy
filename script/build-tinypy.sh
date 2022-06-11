working_dir=$(pwd)
echo "当前目录: $working_dir"

cd test/tinypy

python2 setup.py tinypy math random time

cp build/tinypy $working_dir/build/

cd $working_dir
