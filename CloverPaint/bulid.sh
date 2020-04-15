progen -o clover.pro.org
cat clover.pro.org progen.template > clover.pro
tmake -o Makefile clover.pro
make
cp ./cloverpaint /home/kubota/InstallCloverPaint/opt/QtPalmtop/bin/

