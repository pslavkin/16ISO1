pkill openocd
openocd -f ./scripts/openocd.cfg \
		-c "init" \
		-c "halt 0" \
		-c "flash write_image erase unlock ./out/16iso1.bin 0x1A000000 bin" \
		-c "reset run" \
		-c "shutdown" 2>&1
./scripts/debug.sh
