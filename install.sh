git clone  https://github.com/epernia/cese-edu-ciaa-template.git .
git checkout 5cfb9cc
git apply sapi.patch
mkdir 16iso1
cd 16iso1
git clone https://github.com/pslavkin/16ISO1.git .
make
./16iso1/scipts/flash.sh
