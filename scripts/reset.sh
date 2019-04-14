pkill openocd
openocd -f ./scripts/openocd.cfg \
		-c "init" \
		-c "halt 0" \
		-c "reset run" \
		-c "shutdown" 2>&1
