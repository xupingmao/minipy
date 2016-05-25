# convert all files to be underscore style as PEP 8

_dirname=$1;

if [ $# -ne 1 ]; then
    echo usage $0 dirname;
    exit 0
fi;

if [ ! -d $_dirname/bak ]; then
    mkdir $_dirname/bak;
fi

for name in `ls $_dirname`; do
    if [ -f $_dirname/$name ]; then
        echo convert $_dirname/$name;
        mv -f $_dirname/$name $_dirname/bak/$name;
        python camel2underscore.py $_dirname/bak/$name > $_dirname/$name;
    fi
done;
