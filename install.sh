cd ~
mkdir examen_pablo_slavkin
cd examen_pablo_slavkin
git clone  https://github.com/epernia/cese-edu-ciaa-template.git . -vv
git checkout 5cfb9cc -b examen_pablo_slavkin
mkdir 16iso1
cd 16iso1
git clone https://github.com/pslavkin/16ISO1.git . -v
git checkout entrega
cp sapi.patch ..
cd ..
git apply sapi.patch -v
make
cd 16iso1
./scripts/flash.sh
