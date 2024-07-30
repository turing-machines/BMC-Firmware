echo "6" > /sys/class/thermal/cooling_device0/cur_state
cd /factory && ./run_all_tests.sh
